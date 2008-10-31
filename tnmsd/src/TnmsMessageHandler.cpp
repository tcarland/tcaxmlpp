#define _TNMSD_TNMSMESSAGEHANDLER_CPP_

#include "TnmsMessageHandler.h"

namespace tnmsd {

TnmsMessageHandler::TnmsMessageHandler ( TnmsTree * tree, TnmsClient * client )
    : _tree(tree), 
      _client(client)
{}

TnmsMessageHandler::~TnmsMessageHandler()
{}


TnmsMessageHandler::AddHandler ( ) 
{}


void  
TnmsMessageHandler::RemoveHandler ( const TnmsRemove  & remove )
{}


void
TnmsMessageHandler::MetricHandler      ( const TnmsMetric  & metric )
{}


void  
TnmsMessageHandler::RequestHandler     ( const TnmsRequest & request )
{}

void  
TnmsMessageHandler::SubscribeHandler   ( const std::string & name )
{}


void 
TnmsMessageHandler::UnsubscribeHandler ( const std::string & name )
{}

void  
TnmsMessageHandler::StructureHandler   ( bool  subscribe )
{}

void  
TnmsMessageHandler::AuthRequestHandler ( const TnmsAuthRequest & request )
{}


void  
TnmsMessageHandler::AuthReplyHandler   ( const TnmsAuthReply   & reply )
{}

void  
TnmsMessageHandler::PingHandler() 
{}


void  
TnmsMessageHandler::PingReplyHandler()
{}



void  
TnmsMessageHandler::LastMessageHandler ( int   record_type ) 
{}


} // namespace

// _TNMSD_TNMSMESSAGEHANDLER_CPP_

