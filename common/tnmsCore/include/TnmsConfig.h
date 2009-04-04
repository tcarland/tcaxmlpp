#ifndef _TNMSCORE_TNMSCONFIG_H_
#define _TNMSCORE_TNMSCONFIG_H_

#include <list>

#include "tcanetpp_types.h"

#include "XmlDocument.h"
using namespace tcaxmlplus;


namespace tnmsCore {

#define TNMS_CONFIG_ROOT "tnms"

/*
 *  sample xml config
 *  <tnms version="0.1"
 *      <tnmsd
 *          agent_name="my agent"
 *          logfile="mylogfile"
 *          syslog="false"
 *          debug="true"
 *          auth_server="authhost"
 *          auth_port="15100">
 *
 *          <server
 *              agent_port="15300"
 *              client_port="15500"
 *              holddown="30"
 *              reconnect="30"/>
 *
 *          <client
 *              name="upstream1"
 *              host="upstreamhost"
 *              port="15301">
 *
 *              <subscribe 
 *                  name="tcanms"/>
 *          <client
 *              name="upstream2"
 *              host="up2host"
 *              port="15302"/>
 *      </tnmsd>
 *  </tnms>
 *
 */


//  Is depth needed?
typedef struct Subscription
{
    std::string  name;
    uint32_t     depth;
} subs_t;


typedef std::list<std::string>  StringList;


struct TnmsClientConfig
{
    std::string  connection_name;
    std::string  hostname;
    ipv4addr_t   hostaddr;
    uint16_t     port;
    uint32_t     holddown_interval;
    uint32_t     reconnect_interval;

    StringList   subs;

    TnmsClientConfig()
        : hostaddr(0),
          port(0),
          holddown_interval(0),
          reconnect_interval(0)
    {}
};

typedef std::list<TnmsClientConfig>  ClientList;


struct TnmsServerConfig
{
    uint16_t     agent_port;
    uint16_t     client_port;
    uint32_t     holddown_interval;
    uint32_t     reconnect_interval;

    TnmsServerConfig()
        : agent_port(0),
          client_port(0),
          holddown_interval(0),
          reconnect_interval(0)
    {}
};

typedef std::list<TnmsServerConfig>         ServerList;


struct TnmsConfig
{
    std::string   agent_name;
    std::string   auth_server;
    uint16_t      auth_port;
    
    std::string   logfile;
    bool          compression;
    bool          syslog;
    bool          debug;

    TnmsServerConfig  serverConfig;
    ClientList        clients;

    TnmsConfig() : auth_port(0) {}
};


typedef std::map<std::string, std::string>  AttributeMap;


class TnmsConfigHandler {

  public:

    TnmsConfigHandler() {}
    TnmsConfigHandler ( const std::string & xmlfilename,
                        const std::string & rootname );

    TnmsConfigHandler ( const char * xmlblob, size_t len,
                        const std::string & rootname );

    virtual ~TnmsConfigHandler();


    virtual bool        parse();


    std::string         getAttribute ( const std::string & key );
    bool                haveAttribue ( const std::string & key );

    void                setDebug ( bool d ) { this->_debug = d; }
    std::string         getErrorStr() const { return _errstr; }


  public:

    TnmsConfig          config;


  protected:

    bool                parseRoot    ( XmlNode * node );
    bool                parseServer  ( XmlNode * node );
    bool                parseClient  ( XmlNode * node );
    XmlNode*            findRootNode ( XmlNode * node, std::string & name );


  protected:

    AttributeMap        _rootAttrs;

    std::string         _xmlcfg;
    bool                _isXML;
    bool                _debug;

    std::string         _rootname;
    std::string         _errstr;

};


} // namespace


#endif  // _TNMSCORE_TNMSCONFIG_H_

