/*
 *  AuthClient.h
 *
 *  Copyright(c) 2008, Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_AUTHCLIENT_H_
#define _TNMSCORE_AUTHCLIENT_H_

#include <map>

#include "MessageHandler.hpp"
#include "TnmsClient.h"

#include "EventManager.h"
#include "EventHandlers.hpp"
using namespace tcanetpp;



namespace tnmsCore {

class AuthClient;


// ---------------------------------------------------------------------------------------

struct AuthAttempt 
{
    TnmsAuthRequest    request;
    TnmsClient*        client;
    time_t             sent_at;

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


// ---------------------------------------------------------------------------------------

class AuthMessageHandler : public MessageHandler {

  public:

    AuthMessageHandler ( AuthClient * client );
    virtual ~AuthMessageHandler() {}

    virtual void  AuthReplyHandler   ( const TnmsAuthReply   & reply );
    virtual void  PingHandler();
    virtual void  PingReplyHandler() {}

  protected:

    AuthClient *  _client;
};


// ---------------------------------------------------------------------------------------

class AuthIOHandler : public EventIOHandler {

  public:

    AuthIOHandler() {}
    virtual ~AuthIOHandler() {}

    virtual void        handle_accept   ( const EventIO & io ) {}
    virtual void        handle_read     ( const EventIO & io );
    virtual void        handle_write    ( const EventIO & io );
    virtual void        handle_shut     ( const EventIO & io ) {}
    virtual void        handle_close    ( const EventIO & io );
    virtual void        handle_destroy  ( const EventIO & io ) {}

    virtual bool        readable        ( const EventIO & io );
    virtual bool        writeable       ( const EventIO & io );
};


// ---------------------------------------------------------------------------------------

class AuthClient : public TnmsClient {

  public:

    AuthClient ( EventManager * evmgr );
    AuthClient ( const std::string & authsvr, uint16_t authport,
                 EventManager * evmgr );
    
    virtual ~AuthClient();


  public:   /* TnmsClient */

    virtual void   close();

    virtual void   AuthReplyHandler ( const TnmsAuthReply & reply );


  public:  /* AuthClient */

    void           timeout          ( const EventTimer & timer );

    int            authConnect();

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

    time_t                      _idleTimeout, _idlet;
    time_t                      _authRetryInterval, _lastConn;

    bool                        _bypass;
};


}  // namespace 


#endif // _TNMSCORE_AUTHCLIENT_H_
