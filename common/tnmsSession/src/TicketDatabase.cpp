#define _TNMSSESSION_TICKETDATABASE_CPP_

#include "TicketDatabase.h"

#include "ThreadThreadAutoMutex.hpp"
using namespace tcanetpp;



namespace tnmsSession {


/*
class AutoMutex {
  public:
    AutoMutex ( ThreadLock * lock )
    {
        this->_mutex = lock;
        this->_mutex->lock();
    }
    ~AutoMutex()
    {
        this->_mutex->unlock();
    }
    
  private:
    ThreadLock *  _mutex;
};
*/



TicketDatabase::TicketDatabase()
    : _lock(new ThreadLock())
{
}


TicketDatabase::~TicketDatabase()
{
    if ( this->_lock )
        delete this->_lock;
}


bool
TicketDatabase::insert ( const std::string & username, 
                         const std::string & ticket,
                         const std::string & ipaddr,
                         const time_t      & now,
                         time_t              refresh_interval )
{
    ThreadAutoMutex mutex(_lock);
    
    if ( this->haveTicket(ticket) ) {
        _errStr = "TicketDatabase::insertTicket: ticket already exists for user:";
        _errStr.append(username + " ticket: " + ticket);
        return false;
    }
    
    struct Ticket *  mTicket = new Ticket();
    
    mTicket->username = username;
    mTicket->ticket   = ticket;
    mTicket->ipaddr   = ipaddr;
    mTicket->expire   = now + refresh_interval;
    mTicket->refresh  = refresh_interval;
    mTicket->timer    = _timers.insert(std::pair<time_t, struct Ticket*>(mTicket->expire, mTicket));
    
    _tickets[mTicket->ticket] = mTicket;
    
    return true;
}

bool
TicketDatabase::refresh ( const std::string & username,
                          const std::string & ticket,
                          const std::string & ipaddr,
                          const time_t      & now )
{
    ThreadAutoMutex  mutex(_lock);
    
    TicketMap::iterator tIter = _tickets.find(ticket);
    
    if ( tIter == _tickets.end() ) {
        _errStr = "TicketDatabase::refreshTicket() ticket not found for user ";
        _errStr.append(username + " ticket: " + ticket);
        return false;
    }
    
    Ticket * mTicket = tIter->second;
    
    if ( mTicket == NULL ) {
        _errStr = "TicketDatabase::refreshTicket() ERROR: ticket not valid";
        _tickets.erase(tIter);
        return false;
    }
    
    if ( mTicket->username != username ) {
        _errStr = "TicketDatabase::refreshTicket() user does not match ticket owner";
        return false;
    }
    
    if ( mTicket->ipaddr.length() != 0  && mTicket->ipaddr != ipaddr ) {
        _errStr = "TicketDatabase::refreshTicket() IP Address mismatch: ticket ip: ";
        _errStr.append(mTicket->ipaddr + " provided: " + ipaddr);
        return false;
    }
    
    mTicket->expire = now + mTicket->refresh;
    _timers.erase(mTicket->timer);
    mTicket->timer = _timers.insert(std::pair<time_t, struct Ticket*>(mTicket->expire, mTicket));
    
    return true;
}

bool
TicketDatabase::expire ( const std::string & username,
                         const std::string & ticket,
                         const std::string & ipaddr )
{
    ThreadAutoMutex autoMutex(_lock);
    
    TicketMap::iterator tIter = _tickets.find(ticket);
    
    if ( tIter == _tickets.end() ) {
        _errStr = "TicketDatabase::expireTicket() Ticket not found user: ";
        _errStr.append(username + " ticket: " + ticket);
        return false;
    }
    
    struct Ticket * mTicket = tIter->second;
    
    if ( mTicket == NULL ) {
        _errStr = "TicketDatabase::expireTicket() ERROR: ticket not valid";
        _tickets.erase(tIter);
        return false;
    }
    
    if ( mTicket->username != username ) {
        _errStr = "TicketDatabase::expireTicket() user does not match ticket owner";
        return false;
    }
    
    if ( mTicket->ipaddr.length() != 0 && mTicket->ipaddr != ipaddr ) {
        _errStr = "TicketDatabase::expireTicket() IP Address mismatch: ticket ip: ";
        _errStr.append(mTicket->ipaddr + " provided: " + ipaddr);
        return false;
    }
    
    _timers.erase(mTicket->timer);
    _tickets.erase(mTicket->ticket);
    
    delete mTicket;
    return true;
}

bool
TicketDatabase::isAuthentic ( const std::string & username,
                              const std::string & ticket,
                              const std::string & ipaddr )
{
    ThreadAutoMutex autoMutex(_lock);
    
    TicketMap::iterator tIter = _tickets.find(ticket);
    
    if ( tIter == _tickets.end() ) {
        _errStr = "TicketDatabase::isAuthentic() Ticket not found user: ";
        _errStr.append(username + " ticket: " + ticket);
        return false;
    }
    
    struct Ticket * mTicket = tIter->second;
    
    if ( mTicket == NULL ) {
        _errStr = "TicketDatabase::expireTicket() ERROR: ticket not valid";
        _tickets.erase(tIter);
        return false;
    }
    
    if ( mTicket->username != username ) {
        _errStr = "TicketDatabase::expireTicket() user does not match ticket owner";
        return false;
    }
    
    if ( ! mTicket->ipaddr.empty() && mTicket->ipaddr != ipaddr ) {
        _errStr = "TicketDatabase::expireTicket() IP Address mismatch: ticket ip: ";
        _errStr.append(mTicket->ipaddr + " provided: " + ipaddr);
        return false;
    }

    return true;
}

void
TicketDatabase::clearStale ( std::list<std::string> & ticketlist, const time_t & now )
{
    ThreadAutoMutex autoMutex(_lock);
    
    TimerSet::iterator  iIter = _timers.begin();
    
    while ( iIter != _timers.end() && iIter->first < now ) {
        Ticket * mTicket = iIter->second;
        ticketlist.push_back(mTicket->ticket);
        
        _tickets.erase(mTicket->ticket);
        
        TimerSet::iterator jIter = iIter;
        ++iIter;
        _timers.erase(jIter);
        
        delete mTicket;
    }
}


bool
TicketDatabase::haveTicket ( const std::string & ticket )
{
    ThreadAutoMutex autoMutex(_lock);
    return(_tickets.find(ticket) != _tickets.end());
}


}  // namespace


// _TNMSSESSION_TICKETDATABASE_CPP_

