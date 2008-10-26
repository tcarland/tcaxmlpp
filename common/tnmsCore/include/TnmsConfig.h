#ifndef _TNMSCORE_TNMSCONFIG_H_
#define _TNMSCORE_TNMSCONFIG_H_

#include <list>


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

typedef struct Subscription
{
    std::string  name;
    uint32_t     depth;
} subs_t;


typedef std::list<std::string>  StringList;

struct TnmsClient
{
    std::string  svrhost;
    ipv4addr_t   hostaddr;
    uint16_t     port;

    StringList   subs;
};

typedef std::list<TnmsMirror>  MirrorList;

struct TnmsServer
{
    uint16_t     agent_port;
    uint16_t     client_port;
    uint32_t     holddown;
    uint32_t     reconnect;

    MirrorList   mirrors;
};

typedef std::list<TnmsServer>  ServerList;


struct TnmsConfig
{
    std::string   agent_name;
    std::string   auth_server;
    uint16_t      auth_port;
    
    std::string   logfile;
    bool          compression;
    bool          syslog;
    bool          debug;

    ServerList    servers;

};


class TnmsConfigHandler {

  public:

    TnmsConfigHandler() {}

    TnmsConfigHandler ( const std::string & filename,
                        const std::string & appname );

    TnmsConfigHandler ( const char * xmlblob, size_t len,
                        const std::string & appname );


    virtual ~TnmsConfigHandler() {}


    virtual bool parse();


    bool         setDebug ( bool d ) { this->_debug = d; }
    std::string  getErrorStr() const { return _errstr; }


  public:

    TnmsConfig   config;


  protected:

    bool         parseRoot();
    bool         parseServer();
    bool         parseClient();
    bool         parseSubs();

  protected:

    std::string         _xmlcfg;
    bool                _isXML;
    bool                _debug;
    std::string         _appname;
    std::string         _errstr;

    bool                _debug;
};


} // namespace


#endif  // _TNMSCORE_TNMSCONFIG_H_

