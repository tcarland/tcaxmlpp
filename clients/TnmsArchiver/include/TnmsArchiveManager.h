#ifndef _TNMSARCHIVE_TNMSARCHIVEMANAGER_H_
#define _TNMSARCHIVE_TNMSARCHIVEMANAGER_H_

#include <map>
#include <string>

#include "EventManager.h"
#include "Socket.h"
using namespace tcanetpp;

#include "tnmsCore.h"
#include "TnmsConfig.h"
#include "TnmsClient.h"
using namespace tnmsCore;


namespace tnmsarchive {


#define LOG_CHECK_INTERVAL         3600
#define DEFAULT_HOLDDOWN_INTERVAL  30
#define DEFAULT_REPORT_INTERVAL    30

#define TNMSARCHIVER_CONFIG_ROOT   "archiver"


class ClientIOHandler;



class TnmsArchiveManager : public EventTimerHandler {

  public:

    TnmsArchiveManager ( const std::string & configfile );

    virtual ~TnmsArchiveManager();


    /* EventTimerHandler */
    virtual void        timeout  ( const EventTimer * timer );
    virtual void        finished ( const EventTimer * timer ) {}


    /*  TnmsArchiveManager */
    void                run();

    void                setAlarm();
    void                setHUP();
    void                setUSR();

    void                setDebug  ( bool d = true );
    bool                getDebug() const;
    const std::string&  getErrorStr() const;

    static std::string  Version();


  protected:

    void                createClients();
    void                destroyClients();

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


  private:

    EventManager*               _evmgr;
    TnmsTree*                   _tree;

    ClientMap                   _clients;
    evid_t                      _reportId, _logId;

    ClientIOHandler*            _clientHandler;

    ArchiverConfig              _tconfig;

    time_t                      _lastTouched;
    time_t                      _reportDelay;
    time_t                      _logCheck;
    time_t                      _holddown;
    int                         _today;

    std::string                 _configfile;
    std::string                 _logname;
    std::string                 _errstr;

    bool                        _hup;
    bool                        _usr;
    bool                        _debug;

};


}  // namespace

#endif //  _TNMSARCHIVE_TNMSARCHIVEMANAGER_H_

