#ifndef _TNMSD_TNMSMESSAGEHANDLER_H_
#define _TNMSD_TNMSMESSAGEHANDLER_H_

#include "MessageHandler.hpp"
#include "TnmsMessage.h"
#include "TnmsMetric.h"
using namespace tnmsCore;


namespace tnmsd {


class TnmsTree;
class TnmsClient;

class TnmsMessageHandler : public MessageHandler {

  public:

    TnmsMessageHandler ( TnmsTree * tree, TnmsClient * client );
    virtual ~TnmsMessageHandler();

        /*  Messaging Callbacks  */
    virtual void  AddHandler         ( const TnmsAdd     & add );
    virtual void  RemoveHandler      ( const TnmsRemove  & remove );
    virtual void  MetricHandler      ( const TnmsMetric  & metric );
    virtual void  RequestHandler     ( const TnmsRequest & request );

    virtual void  SubscribeHandler   ( const std::string & name );
    virtual void  UnsubscribeHandler ( const std::string & name );
    virtual void  StructureHandler   ( bool  subscribe );

    virtual void  AuthRequestHandler ( const TnmsAuthRequest & request );
    virtual void  AuthReplyHandler   ( const TnmsAuthReply   & reply );

    virtual void  PingHandler() {}
    virtual void  PingReplyHandler() {}

    virtual void  LastMessageHandler ( int   record_type );


  protected:

    TnmsTree *    _tree;
    TnmsClient *  _client;
};


} // namespace

#endif  //  _TNMSD_TNMSMESSAGEHANDLER_H_

