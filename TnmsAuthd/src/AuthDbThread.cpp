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

    SqlSessionInterface * sql = _dbpool->acquire();
    if ( sql == NULL ) {
        LogFacility::LogMessage("AuthDbThread::authenticate() ERROR acquiring DB instance" 
            + _dbpool->getErrorStr());
        return false;
    }

    TnmsDbUser * userdb = NULL;
    
    userdb = this->findUser(username);
    if ( userdb == NULL )
        userdb = this->queryUser(sql, username);

    if ( userdb == NULL ) {
        return false;
    } else {
        userdb->last = now;
    }
     
    result = this->authenticateUser(userdb, password);

    while ( result && retry++ < TICKET_MAX_RETRY )
    {
        _ticketProvider->getRandomString(TNMS_TICKET_LENGTH, ticket);

        ticket = _ticketDb->insert(username, ticket, ipaddr, now
            TICKET_REFRESH_INTERVAL + TICKET_GRACE_PERIOD);

        if ( ticket )
            break;
    }

    if ( ticket )
        this->dbStoreTicket(sql, userdb);

    return true;
}


bool
AuthDbThread::isAuthentic ( const std::string & username,
                            const std::string & ticket,
                            const std::string & ipaddr )
{
    bool result = false;

    result = _ticketDb->isAuthentic(username, ticket, ipaddr);

    LogFacility::Message  logmsg;
    logmsg << "AuthDbThread::isAuthentic() " << username << "@"
           << ipaddr << " = " result;
    LogFacility::LogMessage(logmsg.str());

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

    if ( result )
        reply.authResult(AUTH_SUCCESS);
    else
        reply.authResult(AUTH_INVALID);

    // add user authorization list to result


    LogFacility::Message  logmsg;
    logmsg << "AuthDbThread::isAuthentic() " << username << "@"
           << ipaddr << " = " result;
    LogFacility::LogMessage(logmsg.str());

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
    // clear User object from cache
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

//----------------------------------------------------------------

eAuthType
AuthDbThread::authenticateUser ( const std::string & username, const std::string & password, const std::string & authmethod )
{
    if ( authmethod.compare(AUTHDB_METHOD_DBSTATIC) == 0 )
    {
        return this->dbAuthUser(username, password);
    }

    tnmsSession::CommandLineAuthenticator  authp(this->getAuthCommand(authmethod));

    return authp->authenticate(username, password);
}

//----------------------------------------------------------------


TnmsDbUser*
AuthDbThread::findUser ( const std::string & username )
{
    ThreadAutoMutex  mutex(_lock);
    TnmsDbUser *     user = NULL;

    AuthUserMap::iterator  uIter = _userMap.find(username);

    if ( uIter != _userMap.end() )
        user = uIter->second;

    return user;
}


TnmsDbAgent*
AuthDbThread::findAgent ( const std::string & agentname )
{
    ThreadAutoMutex  mutex(_lock);
    TnmsDbAgent *    agent = NULL;

    AuthAgentMap::iterator  aIter = _agentMap.find(agentname);

    if ( aIter != _agentMap.end() )
        agent = aIter->second;

    return agent;
}

//----------------------------------------------------------------

TnmsDbUser*
AuthDbThread::queryUser ( SqlSessionInterface * session,
                          const std::string   & username )
{
    TnmsDbUser * user = NULL;
    
    SqlSession * sql = (SqlSession*) session;
    Query        query = sql->newQuery();
    Result       res;
    Row          row;

    query << "SELECT u.uid, u.gid, u.authtype_id, u.username, u.internal "
          << "g.name, m.method_name, m.authbin_name "
          << "FROM tnms.users u JOIN tnms.groups g ON g.gid = u.gid "
          << "JOIN tnms.auth_types m ON m.authtype_id = u.authtype_id "
          << "WHERE username=\"" << username << "\"";

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::queryUser SQL error: " 
            + sql->getErrorStr());
        return NULL;
    } else if ( res.size() == 0 ) {
        LogFacility::LogMessage("AuthDbThread::queryUser() user not found "
            + username);
        return NULL;
    }

    Result::iterator  rIter = res.begin();
    row  = (Row) *rIter;
    user = new TnmsDbUser();

    user->uid         = SqlSession::fromString<uint32_t>(std::string(row[0]));
    user->gid         = SqlSession::fromString<uint32_t>(std::string(row[1]));
    user->internal    = SqlSession::fromString<bool>(std::string(row[4]));
    user->auth_method = std::string(row[6]);
    user->auth_bin    = std::string(row[7]);

    return user;
}


TnmsDbFilter*
AuthDbThread::queryAuthFilter ( SqlSessionInterface * session, uint32_t gid )
{
    TnmsDbFilter*  filter = NULL;
    SqlSession*    sql    = (SqlSession*) session;
    Query          query  = sql->newQuery();
    Result         res;
    Row            row;
    uint32_t       sid;

    query << "SELECT subtree_id FROM tnms.group_authorizations WHERE gid=" << gid;

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::queryAuthFilter() SQL error: "
            + sql->getErrorStr());
        return NULL;
    } else if ( res.size() == 0 ) {
        return NULL;
    }

    filter = new TnmsDbFilter();

    Result::iterator   rIter;
    for ( rIter = res.begin(); rIter != res.end(); ++rIter ) {
        row = (Row) *rIter;

        sid = SqlSession::fromString<uint32_t>(std::string(row[0]));

        this->querySubtree(session, sid, filter);
    }

    ThreadAutoMutex  mutex(_lock);
    _authFilter[gid] = filter;

    return filter;
}


bool
AuthDbThread::querySubtree ( SqlSessionInterface * session, uint32_t sid, TnmsDbFilter * filter )
{
    SqlSession*  sql = (SqlSession*) session;
    Query        query = sql->newQuery();
    Result       res;
    Row          row;
    std::string  name;
    bool         inc;

    query << "SELECT subtree_name, isInclude FROM tnms.authorizations WHERE subtree_id=" << sid;

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::querySubtree() SQL error: " + sql->getErrorStr());
        return false;
    } else if ( res.size() == 0 ) {
        return true;
    }

    Result::iterator   rIter;

    rIter  = res.begin();
    row    = (Row) *rIter;

    name   = std::string(row[0]);
    inc    = SqlSession::fromString<bool>(std::string(row[1]));

    if ( filter->authorizations.size() == 0 || filter->isInclude == inc ) {
        filter->authorizations.push_back(name);
        filter->isInclude = inc;
    } else { 
        LogFacility::LogMessage("AuthDbThread::querySubtree() Error: filter type mismatch");
        return false;
    }

    return true;
}


TnmsDbAgent*
AuthDbThread::queryAgent ( SqlSessionInterface * session, const std::string & agentname )
{
    TnmsDbAgent*   agent = NULL;
    SqlSession*    sql   = (SqlSession*) session;
    Query          query = sql->newQuery();
    Result         res;
    Row            row;

    query << "SELECT agent_id, gid, ipaddress, parent_name, required " 
          << "FROM tnms.agents WHERE name=\"" << agentname << "\"";

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::queryAgent() SQL error: " + sql->getErrorStr());
        return agent;
    } else if ( res.size() == 0 ) {
        return agent;
    }

    Result::iterator  rIter;

    agent = new TnmsDbAgent();
    rIter = res.begin();
    row   = (Row) *rIter;

    agent->agent_id = SqlSession::fromString<uint32_t>(std::string(row[0]));
    agent->gid      = SqlSession::fromString<uint32_t>(std::string(row[1]));
    agent->name     = agentname;
    agent->ipaddr   = std::string(row[2]);

    this->queryAgentConfig(session, agent);

    ThreadAutoMutex  mutex(_lock);

    _agentMap[agentname] = agent;

    return agent;
}


bool
AuthDbThread::queryAgentConfig ( SqlSessionInterface * session, TnmsDbAgent * agent )
{
    SqlSession*    sql   = (SqlSession*) session;
    Query          query = sql->newQuery();
    Result         res;
    Row            row;

    query << "SELECT agent_config FROM tnms.agent_configs WHERE agent_id=" 
          << agent->agent_id;

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::queryAgent() SQL error: " + sql->getErrorStr());
        return false;
    } else if ( res.size() == 0 ) {
        return true;
    }

    Result::iterator  rIter = res.begin();
    row = (Row) *rIter;

    agent->agent_config = std::string(row[0]);
    
    return true;
}


bool
AuthDbThread::dbAuthUser ( const std::string & username, const std::string & password )
{
    return true;
}


bool
AuthDbThread::dbStoreTicket ( SqlSessionInterface * session, TnmsDbUser * user )
{    
    SqlSession*    sql   = (SqlSession*) session;
    Query          query = sql->newQuery()

    query << "INSERT INTO tnms.tickets (username, ticket, ipaddress) "
          << "VALUES (\"" << user->username << "\", \"" << sql->escapeString(user->ticket)
          << "\", \"" << user->ipaddr << "\")";

    if ( ! sql->submitQuery(query) ) {
        LogFacility::LogMessage("AuthDbThread::storeTicket() SQL error: " + sql->getErrorStr());
        return false;
    }

    return true;
}


bool
AuthDbThread::dbRestoreTickets ( SqlSessionInterface * session, const time_t & now )
{    
    SqlSession*    sql   = (SqlSession*) session;
    Query          query = sql->newQuery();
    Result         res;
    Row            row;
    std::string    user, ticket, ipaddr;

    query << "SELECT username, ticket, ipaddress FROM tnms.tickets";

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::restoreTickets() SQL error: "
            + sql->getErrorStr());
        return false;
    }

    Result::iterator  rIter;

    for ( rIter = res.begin(); rIter != res.end(); ++rIter )
    {
        row = (Row) *rIter;

        user    = std::string(row[0]);
        ticket  = std::string(row[1]);
        ipaddr  = std::string(row[2]);

        _ticketDb->insert(user, ticket, ipaddr, now, TICKET_REFRESH, TICKET_GRACE_TIME);
    }

    return true;
}

bool
AuthDbThread::dbClearTickets ( SqlSessionInterface * session, StringList & stales )
{
    SqlSession*    sql   = (SqlSession*) session;
    Query          query;


    if ( stales.size() == 0 ) 
    {
        query = sql->newQuery();
        // This is highly inefficient. DROP/CREATE TABLE is preferred, but this
        // function should be very rarely called, and authdb perf at the time 
        // this would be used shouldn't matter, so we live with this.
        query << "DELETE FROM tnms.tickets"; 
        sql->submitQuery(query);
        return true;
    }

    StringList::iterator  sIter;
    for ( sIter = stales.begin(); sIter != stales.end; ++sIter )
    {
        query = sql->newQuery();
        query << "DELETE FROM tnms.tickets WHERE ticket=\""
              << sql->escapeString(*sIter) << "\"";
        sql->submitQuery(query);
    }

    return true;
}


}  // namespace


//  _TNMSAUTH_AUTHDBTHREAD_CPP_

