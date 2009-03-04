#ifndef _TNMSCORE_TNMSBASE_H_
#define _TNMSCORE_TNMSBASE_H_

#include <string>
#include <map>
#include <set>

#include "TnmsMetric.h"
#include "TnmsOid.h"
#include "HeirarchicalStringTree.hpp"


using namespace tnmsCore;

namespace tnmsCore {



typedef HeirarchicalStringTree<TnmsMetric*>  MetricTree;

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

    void    reconnect ( time_t  secs );
    time_t  reconnect();

    void    max_messages ( int  max );
    int     max_messages();

    void    debug     ( bool debug );
    void    syslog    ( int facility );
    void    logfile   ( const std::string & logfilename );

    bool    need_flush();
    size_t  flushsize();


    static 
    const std::string&  Version();

  private:

    bool    doInput   ( const time_t & now );
    bool    doOutput  ( const time_t & now );
    bool    sendTree  ( const time_t & now);

    void    openLog   ( const std::string & logfile );

    int     checkConfig();
    void    reconfigure();


  private:

    std::string          _agentName;

    MetricTree           _tree;
    UpdateSet            _updates;
    StringSet            _removes;

    std::string          _xmlConfig;
    std::string          _configName;
    std::string          _hostName;
    uint16_t             _hostPort;

    time_t               _holddown;
    time_t               _reconTime;
    time_t               _configTime;
    int                  _maxMessages;
    int                  _today;
    bool                 _subtree;
    bool                 _debug;

    static 
    const std::string    ApiVersion;
   
};


}  // namespace


#endif  // _TNMSCORE_TNMSBASE_H_
