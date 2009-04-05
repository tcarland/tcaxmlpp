#ifndef _TNMSD_TNMSMANAGER_H_
#define _TNMSD_TNMSMANAGER_H_

#include <map>
#include <string>

#include "TnmsTree.h"
#include "AgentIOHandler.h"
#include "ClientIOHandler.h"

#include "EventManager.h"
#include "Socket.h"
using namespace tcanetpp;

#include "TnmsConfig.h"
using namespace tnmsCore;


namespace tnmsd {


#define LOG_ROTATE_INTERVAL        86400
#define DEFAULT_STARTUP_DELAY      30
#define DEFAULT_HOLDDOWN_INTERVAL  30

#define TNMSD_CONFIG_ROOT          "tnmsd"


class TnmsClient;
class TnmsAuthClient;

typedef std::map<evid_t, TnmsClient*>  ClientMap;



class TnmsManager : public EventTimerHandler {

  public:

    TnmsManager ( const std::string & configfile );
    virtual ~TnmsManager();

    // EventTimerHandler
    virtual void timeout ( const EventTimer * timer );

    void         run();  

    void         setAlarm();
    void         setHUP();
    void         setUSR();

    void         setDebug  ( bool d = true );
    std::string  getErrorStr();

  protected:

    void         createClients();
    void         verifyClients ( const time_t      & now );
    bool         parseConfig   ( const std::string & cfg,
                                 const time_t      & now );

    void         logRotate     ( std::string         logfile,
                                 const time_t      & now );


  private:

    EventManager*       _evmgr;
    TnmsTree*           _tree;

    Socket*             _server;
    Socket*             _client;
    TnmsAuthClient*     _auth;

    ClientMap           _clientMap;
    evid_t              _svrid, _clid;

    AgentIOHandler*     _agentHandler;
    ClientIOHandler*    _clientHandler;
    //TnmsMessageHandler  _msgHandler;

    TnmsConfig          _tconfig;

    time_t              _lastTouched;
    time_t              _logRotate;
    time_t              _startDelay, _startat;
    int                 _today;

    std::string         _configfile;
    std::string         _errstr;

    bool                _hup;
    bool                _usr;
    bool                _debug;

};


}  // namespace

#endif //  _TNMSD_TNMSMANAGER_H_

