#ifndef _TNMSAUTH_AUTHDBTHREAD_H_
#define _TNMSAUTH_AUTHDBTHREAD_H_

#include "Thread.h"
#include "ThreadLock.h"
using namespace tcanetpp;

#include "SqlDbPool.h"
using namespace tcasqlpp;

#include "TnmsMessage.h"
#include "TnmsAuthReply.h"
using namespace tnmsCore;

#include "TicketDatabase.h"
#include "RandomStringDevice.h"
using namespace tnmsSession;

#include "AuthDbConfig.hpp"


namespace tnmsauth {



class AuthDbThread : public tcanetpp::Thread {

  public:

    AuthDbThread ( SqlSessionInterface * sql ) {}

    virtual ~AuthDbThread() {}


    virtual void run();


    /*  Authentication methods  */

    bool          authenticate     ( const std::string & username, 
                                     const std::string & password,
                                     const std::string & ipaddr,
                                     const time_t      & now,
                                     std::string       & ticket );

    bool          isAuthentic      ( const std::string & username,
                                     const std::string & ticket,
                                     const std::string & ipaddr );
 
    bool          isAuthentic      ( const std::string & username,
                                     const std::string & ticket,
                                     const std::string & ipaddr,
                                     TnmsAuthReply     & reply );

    bool          refreshTicket    ( const std::string & username,
                                     const std::string & ticket,
                                     const std::string & ipaddr,
                                     const time_t      & now );
    
    bool          expireTicket     ( const std::string & username,
                                     const std::string & ticket,
                                     const std::string & ipaddr );
 
    /*  retrieves list of currently accepted authentication types */

    bool          getAuthTypes     ( StringList        & authtypes );

    bool          agentIsAuthentic ( const std::string & agentname,
                                     const std::string & ipaddr,
                                     TnmsAuthReply     & reply );

    /*  db pool attributes */
    void          setMinConns      ( int conns );
    void          setMaxConns      ( int conns );


  protected:

    eAuthType     authenticateUser ( TnmsDbUser          & username,
                                     const std::string   & password );

    TnmsDbUser*   findUser         ( const std::string   & username );
    TnmsDbAgent*  findAgent        ( const std::string   & agentname );

    /*   Database methods */
    TnmsDbFilter* queryAuthFilter  ( uint32_t  gid );

    TnmsDbUser*   queryUser        ( SqlSessionInterface * session,
                                     const std::string   & username );


    TnmsDbAgent*  queryAgent       ( SqlSessionInterface * session,
                                     const std::string   & agentname );
    bool          queryAgentConfig ( SqlSessionInterface * session, 
                                     TnmsDbAgent         * agent );


    bool          querySubtree     ( SqlSessionInterface * session,
                                     uint32_t              sid,
                                     TnmsDbFilter        * filter );

    bool          dbAuthUser       ( const std::string   & username,
                                     const std::string   & password );

    bool          dbStoreTicket    ( SqlSessionInterface * session,
                                     TnmsDbUser          * user );

    bool          dbRestoreTickets ( SqlSessionInterface * session,
                                     const time_t        & now );
    bool          dbClearTickets   ( SqlSessionInterface * session,
                                     StringList          & stales );


  private:

    typedef std::map<std::string, TnmsDbUser*>   AuthUserMap;
    typedef std::map<std::string, TnmsDbAgent*>  AuthAgentMap;
    typedef std::map<uint32_t, TnmsDbFilter*>    AuthFilterMap;


  private:

    tcasqlpp::SqlDbPool*                _dbpool;
    tcanetpp::ThreadLock*               _lock;
    tnmsSession::TicketDatabase*        _ticketDb;
    tnmsSession::RandomStringDevice*    _ticketGen;

    AuthUserMap                         _userMap;
    AuthAgentMap                        _agentMap;
    AuthFilterMap                       _filterMap;
    
};

}  // namespace

#endif  //  _TNMSAUTH_AUTHDBTHREAD_H_
