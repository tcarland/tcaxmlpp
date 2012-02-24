#ifndef _TNMSD_TNMSMANAGER_H_
#define _TNMSD_TNMSMANAGER_H_

#include <map>
#include <string>

#include "EventManager.h"
#include "Socket.h"
using namespace tcanetpp;

#include "tnmsCore.h"
#include "TnmsConfig.h"
#include "TnmsClient.h"
#include "TnmsTree.h"
using namespace tnmsCore;


namespace tnmsd {


#define LOG_CHECK_INTERVAL         3600
#define DEFAULT_STARTUP_DELAY      30
#define DEFAULT_HOLDDOWN_INTERVAL  30
#define DEFAULT_REPORT_INTERVAL    30

#define TNMSD_CONFIG_ROOT          "tnmsd"
#define TNSMD_TREESZ_METRIC        "numOfElements"
#define TNMSD_VERSION_METRIC       "version"
#define TNMSD_REPORTI_METRIC       "reportInterval"
#define TNMSD_HOLDDOWN_METRIC      "holddownInterval"


class AgentIOHandler;
class ClientIOHandler;



class TnmsManager : public EventTimerHandler {

  public:

    TnmsManager ( const std::string & configfile );

    virtual ~TnmsManager();


    /* EventTimerHandler */
    virtual void        timeout  ( const EventTimer & timer );
    virtual void        finished ( const EventTimer & timer ) {}

    /*  TnmsManager */
    void                run();

    void                setAlarm();
    void                setHUP();
    void                setUSR();

    void                setDebug   ( bool d = true );
    bool                getDebug() const;
    void                setVerbose ( bool v = true );
    bool                getVerbose() const;
                        
    const std::string&  getErrorStr() const;

    static std::string  Version();


  protected:

    void                createClients();
    void                destroyClients();

    void                reportStats   ( const time_t & now );
    void                addStaticMetrics();

    bool                parseConfig   ( const std::string & cfg,
                                        const time_t & now );

    void                logRotate     ( std::string    logfile,
                                        const time_t & now );

  protected:


    struct MirrorConnection {
        evid_t              id;
        TnmsClientConfig    config;
        TnmsClient *        client;

        MirrorConnection() : id(0), client(NULL) {}

        MirrorConnection ( evid_t & id_, 
                           TnmsClientConfig & config_,
                           TnmsClient * client )
            : id(id_),
              config(config_),
              client(client)
        {}
    };

    typedef std::map<std::string, MirrorConnection>  ClientMap;

    typedef std::set<Socket*>   ServerSet;


  private:

    EventManager*               _evmgr;
    TnmsTree*                   _tree;

    Socket*                     _agent;
    Socket*                     _client;
    AuthClient*                 _auth;

    ServerSet                   _agents;
    ClientMap                   _clients;
    evid_t                      _agentId, _clientId;
    evid_t                      _reportId, _logId;

    AgentIOHandler*             _agentHandler;
    ClientIOHandler*            _clientHandler;

    TnmsConfig                  _tconfig;

    TnmsMetric                  _treeSzm;

    time_t                      _lastTouched;
    time_t                      _reportDelay;
    time_t                      _startDelay, _startAt;
    time_t                      _logCheck;
    time_t                      _holddown;
    int                         _today;

    std::string                 _configfile;
    std::string                 _logname;
    std::string                 _errstr;

    bool                        _hup;
    bool                        _usr;
    bool                        _debug;
    bool                        _verbose;

    static std::string          _Version;
};


}  // namespace

#endif //  _TNMSD_TNMSMANAGER_H_

