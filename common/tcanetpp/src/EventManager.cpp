/**  EventManager
  * 
  *   Class for handling multiplexing data I/O and timer events.
  * 
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#define _TCANETPP_EVENTMANAGER_CPP_

extern "C" {
#ifndef WIN32
# include <sys/select.h>
# include <unistd.h>
#endif
}
#include <algorithm>

#include "EventManager.h"


namespace tcanetpp {


#ifdef WIN32
int EventManager::_maxfdp = 0;
#else
int EventManager::_maxfdp = MAX_FDVAL;
#endif


class FindStaleIOEvents 
{
    explicit FindStaleIOEvents() {}

    bool operator() ( const EventIO & io )
    {
        return io.enabled;
    }
};



/**  @param dieoff  boolean indicates whether to terminate when
  *  there are no subscribed clients or timers left to execute */
EventManager::EventManager ( bool dieoff )
    : _evid(0),
      _minevu(DEFAULT_EVU),
      _dieoff(dieoff),
      _alarm(false), 
      _debug(false)
{}


EventManager::~EventManager() 
{}


evid_t
EventManager::addTimerEvent ( EventTimerHandler * handler, 
			      uint32_t sec, uint32_t msec, int count )
{
    EventTimer  timer;

    if ( (handler == NULL) || (sec == 0 && msec == 0) ) {
        _errstr = "EventManager::addTimerEvent: invalid parameters";
	return 0;
    }

    timer.evid    =  this->getNewEventId();

    if ( timer.evid == 0 )
        return 0;

    timer.evmgr   = this;
    timer.handler = handler;
    timer.count   = count;
    timer.enabled = true;

    if ( sec > 0 ) {
	timer.evsec  = sec;
	timer.abstime.tv_sec  = (long)(time(NULL) + timer.evsec);
	timer.abstime.tv_usec = 0;
    } else {
	timer.evusec = msectoevu(msec);
	timer.abstime.tv_sec  = 0;
	timer.abstime.tv_usec = timer.evusec;
    }
    this->checkMinTimer(timer);

    _timers[timer.evid] = timer;

#   ifdef EV_DEBUG
    printf("id %d:  %u sec - %u msec - %d count\n", timer.evid, sec, msec, count);
#   endif 
    
    return timer.evid;
}


evid_t
EventManager::addTimerEvent ( EventTimerHandler * handler, time_t abstime )
{
    EventTimer  timer;

    if ( handler == NULL || abstime < time(NULL) )
	return 0;

#   ifdef EV_DEBUG
    printf("EventManager::addTimerEvent()\n");
#   endif
    
    timer.evid      = this->getNewEventId();

    if ( timer.evid == 0 )
        return 0;

    timer.evmgr     = this;
    timer.handler   = handler;
    timer.count     = 1;
    timer.evsec     = (long) abstime;
    timer.absolute  = true;
    timer.enabled   = true;

    timer.abstime.tv_sec  = 0;
    timer.abstime.tv_usec = 0;

    _timers[timer.evid] = timer;

    return timer.evid;
}



evid_t
EventManager::addIOEvent ( EventIOHandler * handler, const sockfd_t & sfd, 
                           void * rock, bool isServer )
{
    EventIO  io;

    if ( handler == NULL ) {
        _errstr = "Invalid event handler";
	return 0;
    }

    if ( ! Socket::IsValidDescriptor(sfd) ) {
        _errstr = "Invalid IO Descriptor";
        return 0;
    }

    io.evid     = this->getNewEventId();
    if ( io.evid == 0 )
        return 0;

    io.evmgr    = this;
    io.handler  = handler;
    io.sfd      = sfd;
    io.rock     = rock;
    io.isServer = isServer;
    io.enabled  = true;

#   if EV_DEBUG
    printf("EventManager::addIOEvent() adding ");
    if ( io.isServer )
        printf("server ");
    printf("socket %d id: %d\n", sfd, io.evid);
#   endif 

    _clients[io.evid] = io;

    return io.evid;
}


bool
EventManager::isValidEvent ( evid_t  id )
{
    EventIOMap::iterator     cIter;
    EventTimerMap::iterator  tIter;

    if ( (cIter = _clients.find(id)) != _clients.end() )
        if ( (cIter->second).enabled )
            return true;

    if ( (tIter = _timers.find(id)) != _timers.end() )
        if ( (tIter->second).enabled )
            return true;

    return false;
}


bool
EventManager::removeEvent ( const evid_t & id )
{
    EventTimerMap::iterator   tIter;
    EventIOMap::iterator      cIter;
   
#   ifdef EV_DEBUG
    printf("EventManager::removeEvent() %u\n", id);	
#   endif

    if ( (tIter = _timers.find(id)) != _timers.end() ) {
        if ( tIter->second.enabled )
            this->destroyEvent(tIter->second);
        _events.erase(id);
        _timers.erase(tIter);
	return true;
    }

    if ( (cIter = _clients.find(id)) != _clients.end() ) {
        if ( tIter->second.enabled ) {
            FD_CLR(cIter->second.sfd, &_rset);
            FD_CLR(cIter->second.sfd, &_wset);
            FD_CLR(cIter->second.sfd, &_xset);
            this->destroyEvent(cIter->second);
        }
        _events.erase(id);
        _clients.erase(cIter);
    	return true;
    }

    return false;
}



void
EventManager::eventLoop()
{
    timeval to, now;
    int	    rdy;

    EventIOMap::iterator   cIter;

#   ifdef EV_DEBUG
    printf("EventManager::eventLoop()\n");
#   endif
    
    while ( ! _alarm ) {

	FD_ZERO(&_rset);
	FD_ZERO(&_wset);
	FD_ZERO(&_xset);

	// set IO events
	for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) {
	    EventIO & io = cIter->second;

	    if ( io.handler->readable(&io) )
		FD_SET(io.sfd, &_rset);

	    if ( io.handler->writeable(&io) )
		FD_SET(io.sfd, &_wset);
	}

        // validate timer events
	this->verifyTimers();

        // set minimum event sleep interval 
	::memset(&to, 0, sizeof(struct timeval));
	to.tv_usec = _minevu;

#       ifdef WIN32
	if ( this->activeClients() == 0 ) {
	    Sleep(evutomsec(_minevu));
	}
#       endif

        EventManager::GetTimeOfDay(now);

        // select on our fdsets
	if ( (rdy = ::select(_maxfdp, &_rset, &_wset, &_xset, &to)) < 0 ) {
#           ifdef WIN32
            int err = WSAGetLastError();
            if ( err == WSAEINTR )
                continue;
#           else
	    if ( errno == EINTR )
		continue;
#           endif
	}

        // handle io events
	for ( cIter = _clients.begin(); cIter != _clients.end(); cIter++ ) {
	    EventIO & io = cIter->second;

	    if ( FD_ISSET(io.sfd, &_rset) ) {
                io.abstime = now;

		if ( io.isServer ) {
		    io.handler->handle_accept(&io);
		} else {
		    io.handler->handle_read(&io);
		}
	    }

	    if ( FD_ISSET(io.sfd, &_wset) ) {
                io.abstime = now;
		io.handler->handle_write(&io);
	    }
	}

        // check for timer events
	this->checkTimers(now);

        // single-shot
	if ( this->activeEvents() == 0 && _dieoff )
	    break;
    }

    // cleanup clients
    for ( cIter = _clients.begin(); cIter != _clients.end(); cIter++ )
	this->destroyEvent(cIter->second);
    _clients.clear();

    // cleanup timers
    this->clearTimers();

#   ifdef EV_DEBUG 
    printf("EventManager::eventLoop() finished\n");
#   endif
    
    return;
}


const EventTimer*
EventManager::findTimerEvent ( const evid_t & id ) const
{
    EventTimerMap::const_iterator  tIter;

    tIter = _timers.find(id);

    if ( tIter == _timers.end() )
        return NULL;

    return &tIter->second;
}


const EventIO*
EventManager::findIOEvent ( const evid_t & id ) const
{
    EventIOMap::const_iterator  iIter;

    iIter = _clients.find(id);

    if ( iIter == _clients.end() )
        return NULL;

    return &iIter->second;
}


size_t
EventManager::activeEvents() const
{
    return( this->activeTimers() + this->activeClients() );
}


size_t
EventManager::activeTimers() const
{
    return _timers.size();
}


size_t
EventManager::activeClients() const
{
    return _clients.size();
}


bool
EventManager::isActive ( evid_t  id )
{
    return this->isValidEvent(id);
}


void
EventManager::verifyTimers()
{
    EventTimerMap::iterator  tIter;

    this->_minevu = DEFAULT_EVU;

    for ( tIter = _timers.begin(); tIter != _timers.end(); ) {
	EventTimer & timer = tIter->second;

	if ( timer.evid == 0 || ! timer.enabled ) {
	    _timers.erase(tIter++);
	} else {
	    if ( timer.evusec > 0 && timer.evusec < this->_minevu )
		this->_minevu = timer.evusec;
	    tIter++;
	}
    }

    return;
}


void
EventManager::checkTimers ( const timeval & now )
{
    EventTimerMap::iterator  tIter;

    for ( tIter = _timers.begin(); tIter != _timers.end(); tIter++ ) {
	EventTimer & timer = tIter->second;

	if ( ! timer.enabled )
	    continue;

	if ( timer.abstime.tv_usec > 0 ) {

	    if ( timer.abstime.tv_usec <= _minevu ) {
		timer.abstime.tv_usec = timer.evusec;
		timer.fired++;
		timer.handler->timeout(&timer);

		if ( timer.count > 0 && timer.fired == timer.count )
		    timer.enabled = false;

	    } else {
		timer.abstime.tv_usec -=  _minevu;
	    }

	} else if ( timer.abstime.tv_sec > 0 && timer.abstime.tv_sec <= now.tv_sec ) {
	    timer.fired++;
	    timer.handler->timeout(&timer);

	    if ( timer.absolute ) {
		timer.enabled = false;
	    } else if ( timer.count > 0 && timer.fired == timer.count ) {
		timer.enabled = false;
	    } else {
		timer.abstime.tv_sec = now.tv_sec + timer.evsec;
	    }

	}
    }

    return;
}


void
EventManager::clearTimers()
{
    EventTimerMap::iterator  tIter;

    for ( tIter = _timers.begin(); tIter != _timers.end(); ++tIter ) {
	EventTimer & timer = tIter->second;
	this->destroyEvent(timer);
    }
    _timers.clear();

    return;
}

void
EventManager::clearStaleEvents()
{
    EventTimerMap::iterator  tIter;

    for ( tIter = _timers.begin(); tIter != _timers.end(); ++tIter )
        if ( ! tIter->second.enabled ) 
            _timers.erase(tIter);

    EventIOMap::iterator  iIter;

    for ( iIter = _clients.begin(); iIter != _clients.end(); ++iIter )
        if ( ! iIter->second.enabled )
            _clients.erase(iIter);

    return;
}


void
EventManager::checkMinTimer ( const EventTimer & timer )
{
    if ( timer.evusec < _minevu )
	_minevu = timer.evusec;

    return;
}


void
EventManager::destroyEvent ( EventIO & io )
{
    if ( io.handler )
	io.handler->handle_destroy(&io);
    io.enabled = false;
    return;
}
  

void
EventManager::destroyEvent ( EventTimer & timer )
{
    if ( timer.enabled )
	timer.handler->finished(&timer);
    timer.enabled = false;
}
    

void
EventManager::setDebug ( bool d )
{
    _debug = d;
}


void
EventManager::setAlarm()
{
    _alarm = true;
}


evid_t
EventManager::getNewEventId()
{
    evid_t  id  = _lastid + 1;

    EventSet::iterator  sIter;
    while ( id != _lastid ) {
        if ( (sIter = _events.find(id)) == _events.end() && id != 0 )
            break;
        id++;
    }

    if ( id == _lastid || id == 0 ) {
        _errstr = "Max Events reached. No more event id's available";
        return 0;
    } else {
        _events.insert(id);
        _lastid = id;
    }

    return id;
}


void
EventManager::GetTimeOfDay ( timeval & t )
{
#   ifdef WIN32
    t.tv_sec = (long) ::time(NULL);
#   else
    ::gettimeofday(&t, 0);
#   endif
    return;
}

//---------------------------------------------------------------//

bool
EventTimer::operator== ( const EventTimer & timer )
{
    if ( this->evsec > 0 )
	return(evsec == timer.evsec);
    return(this->evusec == timer.evusec);
}


bool
EventTimer::operator< ( const EventTimer & timer )
{
    if ( this->evsec > 0 )
	return(this->evsec < timer.evsec);
    return(this->evusec < timer.evusec);
}


bool
EventIO::operator== ( const EventIO & io )
{
    return(this->evid == io.evid);
}


} // namespace 

//  _TCANETPP_EVENTMANAGER_CPP_

