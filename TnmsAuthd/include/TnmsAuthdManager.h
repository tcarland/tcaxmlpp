#ifndef TNMSAUTH_TNMSAUTHDMANAGER_H_
#define TNMSAUTH_TNMSAUTHDMANAGER_H_

#include "Socket.h"
#include "EventManager.h"
using namespace tcanetpp;

#include "TnmsTree.h"
using namespace tnmsCore;

#include "SqlSession.hpp"
using namespace tcasql;


#include "AuthConfig.hpp"


namespace tnmsauth {


#define LOG_CHECK_INTERVAL         3600
#define DEFAULT_STARTUP_DELAY      30
#define DEFAULT_HOLDDOWN_INTERVAL  30
#define DEFAULT_REPORT_INTERVAL    30


#define TNMSAUTHD_CONFIG_ROOT  "tnmsauthd"


class SoapIOHandler;
class AuthIOHandler;
class AuthDbThread;


class TnmsAuthdManager : public EventTimerHandler {

  public:

    TnmsAuthdManager();
    virtual ~TnmsAuthdManager();


    // EventTimerHandler
    virtual void timeout ( const EventTimer * timer );


    void         run();


    void         setAlarm();    
    void         setHUP();
    void         setUSR();


    void         setDebug ( bool debug = true );
    bool         getDebug() const { return _debug; }

    std::string  getErrorStr();

  protected:

    bool         parseConfig ( const std::string & cfg,
                               const time_t      & now );

    void         logRotate   ( std::string     logfile,
                               const time_t  & now );

  private:

    EventManager*       _evmgr;
    TnmsTree*           _tree;
    AuthDbThread*       _authDb;
    SqlSession*         _sql;

    Socket*             _svr;
    SoapClient*         _soap;

    evid_t              _svrId, _soapId;
    evid_t              _reportId; _logId;


    SoapIOHandler*      _soapHandler;
    AuthIOHandler*      _authHandler;

    AuthConfig          _aconfig;
    TnmsConfig          _tconfig;

    time_t              _lastTouched;
    time_t              _reportDelay;
    time_t              _logCheck;
    time_t              _holddown;
    int                 _today;

    std::string         _configfile;
    std::string         _logname;
    std::string         _errstr;

    bool                _hup;
    bool                _usr;
    bool                _debug;

};

  
}  // namespace

#endif  //  TNMSAUTH_TNMSAUTHDMANAGER_H_

