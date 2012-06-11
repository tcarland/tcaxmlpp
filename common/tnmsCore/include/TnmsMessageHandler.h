/*
 *  TnmsMessageHandler.h
 *
 *  Copyright(c) 2008,2009  Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_TNMSMESSAGEHANDLER_H_
#define _TNMSCORE_TNMSMESSAGEHANDLER_H_

#include "MessageHandler.hpp"


namespace tnmsCore {


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
    
    virtual void  LastMessageHandler ( int   record_type );

    virtual void  PingHandler();
    virtual void  PingReplyHandler();

  protected:

    TnmsTree *    _tree;
    TnmsClient *  _client;
};


} // namespace

#endif  //  _TNMSCORE_TNMSMESSAGEHANDLER_H_
