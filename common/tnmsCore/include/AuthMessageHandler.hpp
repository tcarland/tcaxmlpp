#ifndef _TNMSCORE_AUTHMESSAGEHANDLER_H_
#define _TNMSCORE_AUTHMESSAGEHANDLER_H_

#include "MessageHandler.hpp"

#include "AuthClient.h"


namespace tnmsCore {



class AuthMessageHandler : public MessageHandler {

  public:

    AuthMessageHandler ( AuthClient * client )
        : _client(client)
    {}

    virtual ~AuthMessageHandler() {}

        /*  Messaging Callbacks  */

    virtual void  AuthReplyHandler   ( const TnmsAuthReply   & reply )
    {
        if ( _client )
            _client->AuthReplyHandler(reply);
    }
    
    virtual void  PingHandler()
    {
        if ( _client )
        {
            TnmsMessage ping(PING_REPLY);
            _client->sendMessage(&ping, true);
        }
    }

    virtual void  PingReplyHandler() {}

  protected:

    AuthClient *  _client;
};


} // namespace

#endif  //  _TNMSCORE_AUTHMESSAGEHANDLER_H_

