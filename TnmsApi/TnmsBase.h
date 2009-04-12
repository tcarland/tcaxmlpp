#ifndef _TNMSCORE_TNMSBASE_H_
#define _TNMSCORE_TNMSBASE_H_

#include <string>
#include <map>
#include <set>


#include "tnmsCore.h"
#include "TnmsConfig.h"


namespace tnmsCore {

#define DEFAULT_RECONFIG_INTERVAL 120


class TnmsTree;
class TnmsClient;


class TnmsBase {

  public:

    TnmsBase ( const std::string & name );

    TnmsBase ( const std::string & name,
               const std::string & host,
               uint16_t            port );

    ~TnmsBase();


    int     send      ( time_t  now );


    bool    add       ( const std::string & name, 
                        const time_t      & now,
                        const std::string & data = 0 );

    bool    remove    ( const std::string & name );
    
    bool    update    ( const std::string & name, 
                        const time_t      & now,
                        uint64_t          & value, 
                        eValueType          type,
                        const std::string & data = 0 );

    bool    update    ( const std::string & name, 
                        const time_t      & now,
                        const std::string & value,
                        const std::string & data = 0 );


    void    clear();
    

    void    holddown_interval  ( time_t  secs );
    time_t  holddown_interval() const;

    void    reconnect_interval ( time_t  secs );
    time_t  reconnect_interval() const;

    void    config_interval    ( time_t  secs );
    time_t  config_interval() const;

    void    flush_limit ( int  max );
    int     flush_limit() const;

    void    set_config    ( const std::string & filename );
    void    set_debug     ( bool debug );
    void    set_syslog    ( int facility );
    void    set_logfile   ( const std::string & logfilename );

    bool    need_flush();
    size_t  flushsize();

    static 
    const std::string&  Version();

  private:

    int     checkConfig       ( const time_t  & now );
    int     checkConnection   ( const time_t  & now );
    int     checkSubscription ( const time_t  & now );
    int     reconfigure       ( const time_t  & now );
    bool    doInput           ( const time_t  & now );
    bool    doOutput          ( const time_t  & now );
    bool    sendTree          ( const time_t  & now );

    void    openLog           ( const std::string & logfile,
                                const time_t  & now);


  private:

    TnmsTree*            _tree;
    std::string          _agentName;

    TnmsClient*          _conn;
    TnmsConfig           _config;

    std::string          _xmlConfig;
    std::string          _configName;
    std::string          _hostName;
    uint16_t             _hostPort;

    time_t               _holddown;
    time_t               _holddown_interval;
    time_t               _reconnect;
    time_t               _reconnect_interval;
    time_t               _reconfig;
    time_t               _reconfig_interval;
    int                  _flushLimit;
    int                  _today;
    bool                 _subscribed;
    bool                 _debug;

    static 
    const std::string    ApiVersion;
   
};


}  // namespace


#endif  // _TNMSCORE_TNMSBASE_H_
