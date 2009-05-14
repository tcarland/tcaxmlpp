#ifndef _TNMSCORE_AUTHCLIENT_H_
#define _TNMSCORE_AUTHCLIENT_H_

#include <map>

#include "TnmsClient.h"

#include "EventManager.h"
using namespace tcanetpp;



namespace tnmsCore {

class AuthIOHandler;


struct AuthAttempt 
{
    TnmsAuthRequest  request;
    TnmsClient*      client;
    time_t           sent_at;

    bool operator== ( const AuthAttempt & attempt ) 
    {
        if ( attempt.request.getElementName().compare(request.getElementName()) == 0 )
            return true;
        return false;
    }

    AuthAttempt()
        : client(NULL), sent_at(0)
    {}
};

typedef std::map<std::string, AuthAttempt>  AuthRequestMap;



class AuthClient : public TnmsClient {

  public:

    AuthClient ( EventManager * evmgr );
    AuthClient ( const std::string & authsvr, uint16_t authport,
                 EventManager * evmgr );
    
    virtual ~AuthClient();


  public:   /* TnmsClient */

    virtual int    connect();
    virtual void   close();

    virtual void   AuthReplyHandler ( TnmsAuthReply & reply );

  public:

    void           timeout          ( const EventTimer * timer );

    bool           authClient       ( TnmsClient * client, TnmsAuthRequest & req );
    void           unauthClient     ( TnmsClient * client );

    void           setAuthServer    ( const std::string & authsvr, uint16_t port );

    evid_t         getEventId()     { return _evid; }
    void           setIdleTimeout   ( const time_t & secs );
    time_t         getIdleTimeout() const;
    void           enableAuthBypass ( bool bypass ) { _bypass = bypass; }


  protected:

    AuthRequestMap              _authMap;
    EventManager *              _evmgr;
    evid_t                      _evid;
    AuthIOHandler *             _authhandler;

    std::string                 _authsvr;
    uint16_t                    _authport;
    time_t                      _idleTimeout, _idlet;
    time_t                      _authRetryInterval;

    bool                        _bypass;
};


}  // namespace 


#endif // _TNMSCORE_AUTHCLIENT_H_
