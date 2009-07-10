#define _TNMSAUTH_AUTHDBTHREAD_CPP_

#include "AuthDbThread.h"
#include "AuthConfig.h"


namespace tnmsauth {




AuthDbThread::AuthDbThread ( SqlSessionInterface * sql )
    : _dbpool(new tcasqlpp::SqlDbPool(sql, NULL)),
      _lock(new tcanetpp::ThreadLock()),
      _ticketDb(new tnmsSession::TicketDatabase()),
      _ticketGen(new tnmsSession::RandomStringDevice())
{
}


AuthDbThread::~AuthDbThread()
{
    delete _dbpool;
    delete _lock;
    delete _ticketDb;
    delete _ticketGen;
}


void
AuthDbThread::run()
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

        _ticketDb->clearStale(stales, now);

        if ( stales.size() > 0 ) {
            this->clearTickets(session, stales);
            stales.clear();
        }

        sleep 1;
    }
    _dbpool->release(sql);

    LogFacility::LogMessage("AuthDbThread finished");

    return;
}


bool
AuthDbThread::authenticate ( const std::string & username, 
                             const std::string & password,
                             const std::string & ipaddr,
                             const time_t      & now,
                             std::string       & ticket )
{
    bool result = false;
    bool ticket = false;
    int  retry  = 0;

    TnmsDbUser * user = this->queryUser(username);

    if ( user == NULL ) 
    {
        return false;
    }
    else 
    {
        result = this->authenticateUser(username, password, user->auth_method);
    }

    while ( result && retry++ < TICKET_MAX_RETRY )
    {
        _ticketProvider->getRandomString(TNMS_TICKET_LENGTH, ticket);

        ticket = _ticketDb->insert(username, ticket, ipaddr, now
            TICKET_REFRESH_INTERVAL + TICKET_GRACE_PERIOD);

        if ( ticket )
            break;
    }

    if ( ticket )
        this->storeTicket(username, ticket, ipaddr);

    return ticket;
}


bool
AuthDbThread::isAuthentic ( const std::string & username,
                            const std::string & ticket,
                            const std::string & ipaddr )
{
    bool result = false;

    result = _ticketDb->isAuthentic(username, ticket, ipaddr);

    return result;

}
 
bool
AuthDbThread::isAuthentic ( const std::string & username,
                            const std::string & ticket,
                            const std::string & ipaddr,
                            TnmsAuthReply     & reply )
{
    bool result = false;

    result = _ticketDb->isAuthentic(username, ticket, ipaddr);

    reply.result = AUTH_SUCCESS;

    // add user authorization list to result

    return result;
}


bool
AuthDbThread::refreshTicket ( const std::string & username,
                              const std::string & ticket,
                              const std::string & ipaddr,
                              const time_t      & now )
{
    return _ticketDb->refresh(username, ticket, now);
}


bool  
AuthDbThread::expireTicket ( const std::string & username,
                             const std::string & ticket,
                             const std::string & ipaddr )
{
    return _ticketDb->expire(username, ticket, ipaddr);
}

//----------------------------------------------------------------

bool          
AuthDbThread::getAuthTypes ( StringList & authtypes )
{
}

bool 
AuthDbThread::agentIsAuthentic ( const std::string & agentname,
                                 const std::string & ipaddr,
                                 TnmsAuthReply     & reply )
{

}

void 
AuthDbThread::setMinConns ( int conns )
{
    if ( conns > _dbpool->maxConnections() )
        conns = _dbpool->maxConnections();
    _dbpool->minConnections(conns);
}

void
AuthDbThread::setMaxConns ( int conns )
{
    if ( conns < _dbpool->minConnections() )
        conns = _dbpool->minConnections() + 1;
    _dbpool->maxConnections(conns);
}


TnmsDbUser*
AuthDbThread::queryUser ( SqlSessionInterface * session,
                                     const std::string   & username )
{
    TnmsDbUser * user = NULL;

    return user;
}

TnmsDbAgent*
AuthDbThread::queryAgent ( SqlSessionInterface * session, const std::string & agentname )
{
    TnmsDbAgent * agent = NULL;

    return agent;
}

std::string
AuthDbThread::queryAgentConfig ( const std::string & agentname )
{
    std::string str;
    return str;
}

bool
AuthDbThread::storeTicket ( const TnmsDbUser * user, const std::string & ticket )
{
    return false;
}

bool
AuthDbThread::restoreTickets ( SqlSessionInterface * session )
{
    return false;
}

bool
AuthDbThread::clearTickets ( SqlSessionInterface * session, StringList & stales )
{
    return false;
}


}  // namespace


//  _TNMSAUTH_AUTHDBTHREAD_CPP_

