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

#include "AuthDbConfig.h"


namespace tnmsauth {


typedef std::list<std::string>  StringList;


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

    TnmsDbUser*   queryUser        ( SqlSessionInterface * session,
                                     const std::string   & username );
    TnmsDbAgent*  queryAgent       ( SqlSessionInterface * session );
    std::string   queryAgentConfig ( const std::string   & agentname );

    bool          storeTicket      ( const TnmsUser      * authuser,
                                     const std::string   & ticket );

    bool          restoreTickets   ( SqlSessionInterface * session );
    bool          clearTickets     ( SqlSessionInterface * session,
                                     StringList          & stales );

    

  private:

    tcasqlpp::SqlDbPool*                _dbpool;
    tcanetpp::ThreadLock*               _lock;
    tnmsSession::TicketDatabase*        _ticketDb;
    tnmsSession::RandomStringDevice*    _ticketGen;
    
};

}  // namespace

#endif  //  _TNMSAUTH_AUTHDBTHREAD_H_
