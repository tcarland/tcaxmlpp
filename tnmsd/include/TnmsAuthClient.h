#ifndef _TNMSD_TNMSAUTHCLIENT_H_
#define _TNMSD_TNMSAUTHCLIENT_H_

#include <time.h>

#include "TnmsSocket.h"
using namespace tnmsCore;

#include "TnmsManager.h"


namespace tnmsd {


struct AuthProxy
{
    tnmsAuthRequest   req;
    TnmsClient       *client;
    bool              requested;
    time_t            reqTime;

    bool operator== ( const AuthProxy & proxy )
    {
        std::string name = "";
        name.assign(proxy.req.agent_name, proxy.req.agent_name_len);
        if ( name.compare(0, name.length(), 
             req.agent_name, req.agent_name_len) == 0 )
            return true;
        return false;
    }

    AuthProxy()
        : client(NULL),
          requested(false),
          reqTime(0)
    {}
};


typedef std::map<std::string, AuthProxy>  ProxyMap;



class TnmsAuthClient : public TnmsSocket, EventIOHandler {

  public:

    TnmsAuthClient ( EventManager * evmgr );
    virtual ~TnmsAuthClient();

    bool    authenticate  ( TnmsClient * client, TnmsAuthRequest & authreq );
    void    removeClient  ( TnmsClient * client );

    void    connectAuth();

    // from TnmsSocket
    void    close();
    void    AuthReplyHandler ( tnmsAuthReply & reply );


    void    timeout  ( const EventTimer * timer );

    // IOHandler
    void    handle_read      ( const EventIO * io );
    void    handle_close     ( const EventIO * io );
    void    handle_write     ( const EventIO * io );
    void    handle_destroy   ( const EventIO * io )  {}

    bool    readable         ( const EventIO * io );
    bool    writeable        ( const EventIO * io );

private:

    ProxyMap            _proxyMap;
    EventManager *      _evmgr;
    evid_t              _evid;

    std::string         _authhost;
    uint16_t            _authport;

    time_t              _idleTimeout;
    time_t              _ctr;

    bool                _bypass;
};

}  // namespace

#endif // _TNMSD_TNMSAUTHCLIENT_H_

