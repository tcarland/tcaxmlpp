#ifndef _TNMSCORE_TNMSBASE_H_
#define _TNMSCORE_TNMSBASE_H_

#include <string>
#include <map>
#include <set>

//#include "TnmsMetric.h"
//#include "TnmsOid.h"
#include "HeirarchicalStringTree.hpp"


namespace tnmsCore {

class TnmsMetric;
class TnmsOid;
class TnmsAgent;


//typedef HeirarchicalStringTree<TnmsMetric*>  MetricTree;

typedef std::map<std::string, TnmsOid*>      OidMap;
typedef std::set<TnmsMetric*>                UpdateSet;
typedef std::set<std::string>                StringSet;



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
                        eValueTypes         type,
                        const std::string & data = 0 );

    bool    update    ( const std::string & name, 
                        const time_t      & now,
                        const std::string & value,
                        const std::string & data = 0 );


    void    clear();
    
    bool    setConfig ( const std::string & filename );


    void    holddown  ( time_t  secs );
    time_t  holddown();

    void    reconnect_interval ( time_t  secs );
    time_t  reconnect_interval();

    void    flush_limit ( int  max );
    int     flush_limit();

    void    debug     ( bool debug );
    void    syslog    ( int facility );
    void    logfile   ( const std::string & logfilename );

    bool    need_flush();
    size_t  flushsize();

    static 
    const std::string&  Version();

  private:

    int     checkConfig();
    int     checkConnection();
    int     checkSubscription();
    int     reconfigure();

    bool    doInput   ( const time_t      & now );
    bool    doOutput  ( const time_t      & now );
    bool    sendTree  ( const time_t      & now );
    void    openLog   ( const std::string & logfile,
                        const time_t      & now);


  private:

    std::string          _agentName;

    TnmsTree             _tree;
    //MetricTree           _tree;
    //UpdateSet            _updates;
    //StringSet            _removes;

    TnmsConfig           _config;
    TnmsClient           _conn;
    //TnmsAgent            _conn;

    std::string          _xmlConfig;
    std::string          _configName;
    std::string          _hostName;
    uint16_t             _hostPort;

    time_t               _holddown;
    time_t               _reconnect;
    time_t               _configTime;
    int                  _flushLimit;
    int                  _today;
    bool                 _subscribed;
    bool                 _debug;

    static 
    const std::string    ApiVersion;
   
};


}  // namespace


#endif  // _TNMSCORE_TNMSBASE_H_
