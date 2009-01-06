#ifndef _TNMSCORE_TCANMSBASE_H_
#define _TNMSCORE_TCANMSBASE_H_

#include <string>
#include <map>


namespace tnmsCore {


struct TreeMetric
{
    TnmsMetric   metric;
    uint16_t     lastId;


    TreeMetric() : lastId(0) {}

    TreeMetric ( const std::string & name )
        : metric(TnmsMetric(name)),
          lastId(0)
    {}

    ~TreeMetric() {}

    void setLastId ( uint16_t id )
    {
        if ( id > lastId )
            lastId = id;
    }
};


typedef HeirarchicalStringTree<TreeMetric>   MetricStringTree;

typedef std::map<std::string, TnmsOid*>      OidMap;
typedef std::set<TnmsMetric*>                UpdateSet;
typedef std::set<std::string>                StringSet;



class TcaNmsBase {

  public:

    TcaNmsBase ( const std::string & name );

    TcaNmsBase ( const std::string & name,
                 const std::string & host,
                 uint16_t            port );

    ~TcaNmsBase();


    int     send      ( time_t  now );


    bool    add       ( const std::string & name, time_t  now );

    bool    remove    ( const std::string & name );
    
    bool    update    ( const std::string & name, time_t  now,
                        uint64_t value, eValueTypes type,
                        const std::string & data = 0 );

    bool    update    ( const std::string & name, time_t  now,
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
    TnmsOid              _agentOid;

    MetricStringTree     _tree;
    UpdateSet            _updates;
    StringSet            _removes;

    OidMap               _oidMap;
    std::string          _xmlConfig;
    std::string          _configName;
    std::string          _hostName;
    uint16_t             _hostPort;

    time_t               _holddown;
    time_t               _reconTime;
    time_t               _configTime;
    bool                 _subtree;
    int                  _maxMessages;
    int                  _today;

    static 
    const std::string    ApiVersion;
   
};


}  // namespace


#endif  // _TNMSCORE_TCANMSBASE_H_
