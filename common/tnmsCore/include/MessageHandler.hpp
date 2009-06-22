#ifndef _TNSMCORE_MESSAGEHANDLER_H_
#define _TNSMCORE_MESSAGEHANDLER_H_


#include "TnmsMessage.h"
#include "TnmsMetric.h"
#include "TnmsAuthRequest.h"
#include "TnmsAuthReply.h"

namespace tnmsCore {


class MessageHandler {

  public:

    virtual ~MessageHandler() {}

    /*  Messaging Callbacks  */

    virtual void  AddHandler         ( const TnmsAdd     & add ) {}
    virtual void  RemoveHandler      ( const TnmsRemove  & remove ) {}
    virtual void  MetricHandler      ( const TnmsMetric  & metric ) {}
    virtual void  RequestHandler     ( const TnmsRequest & request ) {}

    virtual void  SubscribeHandler   ( const std::string & name ) {}
    virtual void  UnsubscribeHandler ( const std::string & name ) {}
    virtual void  StructureHandler   ( bool  subscribe ) {}

    virtual void  AuthRequestHandler ( const TnmsAuthRequest & request ) {}
    virtual void  AuthReplyHandler   ( const TnmsAuthReply   & reply ) {}

    virtual void  PingHandler() {}
    virtual void  PingReplyHandler() {}

    virtual void  LastMessageHandler ( int   record_type ) {}

};

}  // namespace

#endif // _TNSMCORE_MESSAGEHANDLER_H_

