/** 
  * @file EventManager.cpp
  *
  *   Class for handling multiplexing data I/O and timer events.
  * 
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author  tcarland@gmail.com
  *
  * @section LICENSE
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
# include <time.h>
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


/**  Predicate for locating events that are no longer valid. */
class FindStaleIOEvents 
{
    explicit FindStaleIOEvents() {}
    bool operator() ( const EventIO & io ) {
        return io.enabled;
    }
};



/**  @param dieoff  boolean indicates whether to terminate when
  *  there are no subscribed clients or timers left to execute */
EventManager::EventManager ( bool dieoff )
    : _evid(0),
      _lastid(0),
      _minevu(DEFAULT_EVU),
      _dieoff(dieoff),
      _alarm(false), 
      _debug(false)
{}


EventManager::~EventManager() 
{}


/**  Creates a new timer event using the provided handler.
  *  The parameters represent the timing interval, not absolute time,
  *  in both seconds and/or microseconds, as well as an optional count
  *  for the number of times the event should fire (0 = forever).
 **/
evid_t
EventManager::addTimerEvent ( EventTimerHandler * handler, 
                              uint32_t sec, uint32_t usec, int count )
{
    EventTimer  timer;

    if ( (handler == NULL) || (sec == 0 && usec == 0) )
    {
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

    timer.evsec  = sec;
    timer.evusec = usec; //msectoevu(msec);

    timer.abstime.tv_sec  = 0;
    timer.abstime.tv_usec = 0;

    this->checkMinTimer(timer);

    _timers[timer.evid] = timer;

#   ifdef EV_DEBUG
    printf("id %lu:  %u sec - %u msec - %d count\n", timer.evid, sec, usec, count);
#   endif 
    
    return timer.evid;
}


/**  Creates a new timer event with the provided @param handler and 
  *  @param abstime  as the absolute time the event will fire, meaning 
  *  the event will fire just once. It is up to the user to ensure the 
  *  absolute time given is in the future relative to the EventManager's 
  *  clock.
 **/
evid_t
EventManager::addTimerEvent ( EventTimerHandler * handler, time_t abstime )
{
    EventTimer  timer;

    if ( handler == NULL )
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


/**  Creates a new IO Event for the provided file descriptor and associated handler.
  *  @param handler is the pointer to an EventIOHandler object.
  *  @param sfd is the file handle to track for the given I/O event.
  *  @param rock is a user provided void* for associating user data with the event.
  *  @param isServer  provides for special handling of the file descriptor and any
  *   reads on this descriptor result in the EventIOHandler::handle_accept() to be 
  *   called instead of the handle_read() function.
 **/
evid_t
EventManager::addIOEvent ( EventIOHandler * handler, const sockfd_t & sfd, 
                           void * rock, bool isServer )
{
    EventIO  io;

    if ( handler == NULL ) 
    {
        _errstr = "Invalid event handler";
        return 0;
    }

    if ( ! Socket::IsValidDescriptor(sfd) ) 
    {
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

    io.abstime.tv_sec  = 0;
    io.abstime.tv_usec = 0;

#   if EV_DEBUG
    printf("EventManager::addIOEvent() adding ");
    if ( io.isServer )
        printf("server ");
    printf("socket %d id: %lu\n", sfd, io.evid);
#   endif 

    _clients[io.evid] = io;

    return io.evid;
}


/**  Removes the event associated to the given event id. */
bool
EventManager::removeEvent ( const evid_t & id )
{
    EventTimerMap::iterator   tIter;
    EventIOMap::iterator      cIter;
   
#   ifdef EV_DEBUG
    printf("EventManager::removeEvent() %lu\n", id);
#   endif

    if ( (tIter = _timers.find(id)) != _timers.end() ) 
    {
        if ( tIter->second.enabled )
            this->destroyEvent(tIter->second);
        _events.erase(id);
        _timers.erase(tIter);
        return true;
    }

    if ( (cIter = _clients.find(id)) != _clients.end() ) 
    {
        if ( cIter->second.enabled ) 
        {
            FD_CLR(cIter->second.sfd, &_rset);
            FD_CLR(cIter->second.sfd, &_wset);
            FD_CLR(cIter->second.sfd, &_xset);
            this->destroyEvent(cIter->second);
        }
        _events.erase(id);
        //_clients.erase(cIter);
            return true;
    }

    return false;
}


/**  The main entry point to the EventManager System. This method blocks
  *  until the EventManager is signaled or terminates on its own.
 **/
void
EventManager::eventLoop()
{
    timeval to, now;
    int     rdy = 0;

    EventIOMap::iterator   cIter;

#   ifdef EV_DEBUG
    printf("EventManager::eventLoop()\n");
#   endif
    
    while ( ! _alarm ) 
    {
        FD_ZERO(&_rset);
        FD_ZERO(&_wset);
        FD_ZERO(&_xset);

        // set IO events
        for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) 
        {
            EventIO & io = cIter->second;

            if ( ! io.enabled )
                continue;

            if ( io.handler->readable(io) )
                FD_SET(io.sfd, &_rset);

            if ( io.handler->writeable(io) )
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


        // select on our fdsets
        if ( (rdy = ::select(_maxfdp, &_rset, &_wset, &_xset, &to)) < 0 ) 
        {
#           ifdef WIN32
            int err = WSAGetLastError();
            if ( err == WSAEINTR )
                continue;
#           else
            if ( errno == EINTR )
                continue;
#           endif
        }

        EventManager::GetTimeOfDay(&now);

        // handle io events
        for ( cIter = _clients.begin(); cIter != _clients.end(); cIter++ ) 
        {
            EventIO & io = cIter->second;

            if ( ! io.enabled )
                continue;

            if ( FD_ISSET(io.sfd, &_rset) ) {
                io.abstime = now;

                if ( io.isServer ) {
                    io.handler->handle_accept(io);
                } else {
                    io.handler->handle_read(io);
                }
            }

            if ( FD_ISSET(io.sfd, &_wset) ) {
                io.abstime = now;
                io.handler->handle_write(io);
            }
        }

        for ( cIter = _clients.begin(); cIter != _clients.end(); ) 
        {
            if ( ! cIter->second.enabled )
                _clients.erase(cIter++);
            else
                ++cIter;
        }

        // check for timer events
        EventManager::GetTimeOfDay(&now);
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


/**  Locates the EventTimer object for the given event id. */
const EventTimer*
EventManager::findTimerEvent ( const evid_t & id ) const
{
    EventTimerMap::const_iterator  tIter;

    tIter = _timers.find(id);

    if ( tIter == _timers.end() )
        return NULL;

    return(&tIter->second);
}


/**  Locates the EventIO object for the given event id. */
const EventIO*
EventManager::findIOEvent ( const evid_t & id ) const
{
    EventIOMap::const_iterator  iIter;

    iIter = _clients.find(id);

    if ( iIter == _clients.end() )
        return NULL;

    return(&iIter->second);
}


/**  Return the number of current active events, both timer and io */
size_t
EventManager::activeEvents() const
{
    return( this->activeTimers() + this->activeClients() );
}


/**  Returns the number of currently active timer events. */
size_t
EventManager::activeTimers() const
{
    return _timers.size();
}

/**  Returns the number of currently active io events. */
size_t
EventManager::activeClients() const
{
    return _clients.size();
}


/**  Checks the validity of an event id. An event id is considered valid
  *  when the event id's associated Event object is currently active 
  *  within the EventManager (ie. exists and ->isEnabled())
 **/
bool
EventManager::isActive ( const evid_t & id ) const
{
    return this->isValidEvent(id);
}


bool
EventManager::isValidEvent ( const evid_t & id ) const
{
    EventIOMap::const_iterator     cIter;
    EventTimerMap::const_iterator  tIter;

    if ( (cIter = _clients.find(id)) != _clients.end() )
        if ( (cIter->second).enabled )
            return true;

    if ( (tIter = _timers.find(id)) != _timers.end() )
        if ( (tIter->second).enabled )
            return true;

    return false;
}


void
EventManager::verifyTimers()
{
    EventTimerMap::iterator  tIter;

    this->_minevu = DEFAULT_EVU;

    for ( tIter = _timers.begin(); tIter != _timers.end(); ) 
    {
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

    for ( tIter = _timers.begin(); tIter != _timers.end(); tIter++ ) 
    {
        EventTimer & timer = tIter->second;

        if ( ! timer.enabled )
            continue;

        if ( timer.abstime.tv_sec == 0 ) 
        {
            timer.abstime = now;
            timer.abstime.tv_sec  += timer.evsec;
            timer.abstime.tv_usec += timer.evusec;
            continue;
        }

        bool fired = false;
       
        if ( timer.abstime.tv_sec < now.tv_sec ) 
        {
            fired = true;
        } 
        else if ( timer.abstime.tv_sec == now.tv_sec ) 
        {
            if ( timer.evusec > 0 ) {
                if  ( timer.abstime.tv_usec <= now.tv_usec )
                    fired = true;
            } else {
                fired = true;
            }
        }

        if ( fired ) 
        {
            timer.abstime = now;
            timer.fired++;

            timer.handler->timeout(timer);

            if ( timer.absolute || (timer.count > 0 && timer.fired == timer.count) ) {
                timer.enabled = false;
            } else {
                timer.abstime.tv_sec = now.tv_sec + timer.evsec;
                timer.abstime.tv_usec = now.tv_usec + timer.evusec;
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
        io.handler->handle_destroy(io);
    io.enabled = false;
    return;
}
  

void
EventManager::destroyEvent ( EventTimer & timer )
{
    if ( timer.enabled )
        timer.handler->finished(timer);
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

    while ( id != _lastid ) 
    {
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


int
EventManager::GetTimeOfDay ( timeval * tv )
{
    int r = 0;
    // TODO: fix WIN32
#   ifdef WIN32
    t->tv_sec = (long) ::time(NULL);
#   else
    r = ::gettimeofday(tv, 0);
#   endif
    return r;
}

//---------------------------------------------------------------//

// TODO: should diff in ms need to be a float? why not int64? double?
// Addl. Note: the float was for ease of transmission and size.
float
EventManager::TimevalToMs ( const timeval * tv )
{
    return( ((float)tv->tv_sec * 1000.0F) + ((float)tv->tv_usec / 1000.0F) );
}


float
EventManager::TimevalDiffMs ( const timeval * t2, const timeval * t1 )
{
    timeval  rtv;
    float    result;

    EventManager::TimevalDiff(t2, t1, &rtv);
    result = EventManager::TimevalToMs(&rtv);

    return result;
}


void
EventManager::TimevalDiff ( const timeval * t2, const timeval * t1, timeval * result )
{
    if ( t2 == NULL || t1 == NULL || result == NULL )
        return;

    result->tv_sec  = t2->tv_sec  - t1->tv_sec;
    result->tv_usec = t2->tv_usec - t1->tv_usec;

    if ( result->tv_usec <= 0 )
    {
        --result->tv_sec;
        result->tv_usec += USEC_PER_SEC;
    }

    return;
}


void
EventManager::TimevalNorm ( struct timeval * tv )
{
    if ( tv == NULL )
        return;

    while ( tv->tv_usec >= USEC_PER_SEC )
    {
        tv->tv_sec++;
        tv->tv_usec -= USEC_PER_SEC;
    }

    return;
}

//---------------------------------------------------------------//

void
EventManager::TimespecDiff ( const timespec * t2, const timespec * t1,
                             timespec * result )
{
    if ( t2 == NULL || t1 == NULL || result == NULL )
        return;

    result->tv_sec  = t2->tv_sec  - t1->tv_sec;
    result->tv_nsec = t2->tv_nsec - t1->tv_nsec;
    EventManager::TimespecNorm(result);

    return;
}

int64_t
EventManager::TimespecDiffNS ( const timespec * t2, const timespec * t1 )
{
    int64_t diff;

    diff  = NSEC_PER_SEC * (int64_t)((int)t2->tv_sec - (int)t1->tv_sec);
    diff += ( (int)t2->tv_nsec - (int)t1->tv_nsec );

    return diff;
}

void
EventManager::TimespecNorm ( timespec * ts )
{
    if ( ts == NULL )
        return;

    while ( ts->tv_nsec >= NSEC_PER_SEC )
    {
        ts->tv_sec++;
        ts->tv_nsec -= NSEC_PER_SEC;
    }
    return;
}

//---------------------------------------------------------------//
#ifdef USE_LIBRT

int
EventManager::GetTimeOfClock ( int clkid, timespec * ts )
{
    return( ::clock_gettime(clkid, ts) );
}

int
EventManager::GetClockResolution ( int clkid, timespec * ts )
{
    return( ::clock_getres(clkid, ts) );
}

void
EventManager::NanoSleep ( int clkid, uint64_t & ns )
{
    struct timespec ts, rem;
    int    err;

    ts.tv_sec  = 0;
    ts.tv_nsec = ns;

    EventManager::TimespecNorm(&ts);

    err = ::clock_nanosleep(clkid, 0, &ts, &rem);

    if ( err == 0 )
        return;

    while ( err == EINTR ) {
        ts   = rem;
        err  = ::clock_nanosleep(clkid, 0, &ts, &rem);
    }

    return;
}

void
EventManager::NanoSleep ( uint64_t & ns )
{
    EventManager::NanoSleep(CLOCK_MONOTONIC, ns);
}
#endif

//---------------------------------------------------------------//


} // namespace 

//  _TCANETPP_EVENTMANAGER_CPP_

