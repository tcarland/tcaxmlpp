#define _TNMSAUTH_TNMSAUTHTHREAD_CPP_



#include "TnmsAuthThread.h"


namespace tnmsauth {


TnmsAuthThread::TnmsAuthThread ( SqlSessionInterface * sql )
    : _dbpool(new SqlDbPool(sql, NULL)),
      _lock(new ThreadLock()),
      _ticketmgr(new TicketRepository())
{
}


TnmsAuthThread::~TnmsAuthThread()
{
    delete _dbpool;
    delete _lock;
    delete _ticketmgr;
}


void
TnmsAuthThread::run()
{
    StringList  stales;

    SqlSessionInterface * sql = _dbpool->acquire();

    if ( sql == NULL )
    {
        LogFacility::LogMessage("AuthDbThread::run() failed to acquire db handle " 
            + _dbpool->getErrorStr());
        return;
    }

    this->restoreTickets(sql);

    while ( ! this->_Alarm )
    {
        now = ::time(NULL);
        _ticketmgr->clearStale(stales, now);
        if ( stales.size() > 0 ) {
            this->clearTickets(session, stales);
            stales.clear();
        }
    }
    _dbpool->release(sql);

    LogFacility::LogMessage("AuthDbThread finished");

    return;
}


bool
TnmsAuthThread::authenticate ( const std::string & username, 
                               const std::string & password,
                               const std::string & ipaddr,
                               const time_t      & now,
                               std::string       & ticket )
{
    bool result = false;
    bool ticket = false;
    int  retry  = 0;

    TnmsUser * user = this->queryUser(username);

    if ( user == NULL ) 
    {
        return false;
    }
    else 
    {
        result = this->authenticateUser(username, password, user->auth_method);
    }

    while ( result && retry++ < TICKETMGR_MAX_RETRY )
    {
        _ticketProvider->getRandomString(TICKETMGR_TICKET_LENGTH, ticket);

        ticket = _ticketmgr->insert(username, ticket, ipaddr, now
            TICKETMGR_REFRESH_INTERVAL + TICKETMGR_GRACE_PERIOD);

        if ( ticket )
            break;
    }

    if ( ticket )
        this->storeTicket(username, ticket, ipaddr);

    return ticket;
}


bool
TnmsAuthThread::isAuthentic ( const std::string & username,
                              const std::string & ticket,
                              const std::string & ipaddr )
{
    bool result = false;

    result = _ticketMgr->isAuthentic(username, ticket, ipaddr);

    return result;

}
 
bool
TnmsAuthThread::isAuthentic ( const std::string & username,
                              const std::string & ticket,
                              const std::string & ipaddr,
                              TnmsReply         & reply )
{
    bool result = false;

    result = _ticketMgr->isAuthentic(username, ticket, ipaddr);

    reply.result = AUTH_SUCCESS;

    // add user authorization list to result

    return result;
}


bool
TnmsAuthThread::refreshTicket ( const std::string & username,
                                const std::string & ticket,
                                const std::string & ipaddr,
                                const time_t      & now )
{
    return _ticketMgr->refresh(username, ticket, now);
}


bool  
TnmsAuthThread::expireTicket ( const std::string & username,
                               const std::string & ticket,
                               const std::string & ipaddr )
{
    return _ticketMgr->expire(username, ticket, ipaddr);
}

bool          
TnmsAuthThread::getAuthTypes ( StringList & authtypes )
{
}

bool 
TnmsAuthThread::agentIsAuthentic ( const std::string & agentname,
                                   const std::string & ipaddr,
                                   TnmsReply         & reply )
{

}

void 
TnmsAuthThread::setMinConns ( int conns )
{
    if ( conns > _dbpool->maxConnections() )
        conns = _dbpool->maxConnections();
    _dbpool->minConnections(conns);
}

void
TnmsAuthThread::setMaxConns ( int conns )
{
    if ( conns < _dbpool->minConnections() )
        conns = _dbpool->minConnections() + 1;
    _dbpool->maxConnections(conns);
}



}  // namespace


//  _TNMSAUTH_TNMSAUTHTHREAD_CPP_

