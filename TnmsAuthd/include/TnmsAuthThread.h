#ifndef _TNMSAUTH_TNMSAUTHTHREAD_H_
#define _TNMSAUTH_TNMSAUTHTHREAD_H_

#include "Thread.h"
#include "ThreadLock.h"
using namespace tcanetpp;

#include "SqlDbPool.h"
using namespace tcasqlplus;


namespace tnmsauth {


class TnmsAuthThread : public Thread {

  public:

    TnmsAuthThread ( SqlSessionInterface * sql ) {}

    virtual ~TnmsAuthThread() {}


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
                                     TnmsReply         & reply );

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
                                     TnmsReply         & reply );

    /*  db pool attributes */
    void          setMinConns      ( int conns );
    void          setMaxConns      ( int conns );


  protected:

    TnmsUser*     queryUser        ( SqlSessionInterface * session,
                                     const std::string   & username );
    TnmsAgent*    queryAgent       ( SqlSessionInterface * session );
    std::string   queryAgentConfig ( const std::string   & agentname );

    bool          storeTicket      ( const TnmsUser      * authuser,
                                     const std::string   & ticket );

    bool          restoreTickets   ( SqlSessionInterface * session );
    bool          clearTickets     ( SqlSessionInterace  * session,
                                     StringList          & stales );

    

  private:

    tcasqlplus::SqlDbPool*        _dbpool;
    tcanetpp::ThreadLock*         _lock;
    ticketmgr::TicketRepository*  _ticketmgr;
    
};

}  // namespace

#endif  //  _TNMSAUTH_TNMSAUTHTHREAD_H_
