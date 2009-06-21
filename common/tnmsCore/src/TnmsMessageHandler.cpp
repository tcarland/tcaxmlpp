#define _TNMSD_TNMSMESSAGEHANDLER_CPP_

#include "TnmsMessageHandler.h"
#include "TnmsClient.h"
#include "TnmsTree.h"

#include "LogFacility.h"
using namespace tcanetpp;



namespace tnmsCore {


TnmsMessageHandler::TnmsMessageHandler ( TnmsTree * tree, TnmsClient * client )
    : _tree(tree), 
      _client(client)
{}


TnmsMessageHandler::~TnmsMessageHandler()
{}


void
TnmsMessageHandler::AddHandler ( const TnmsAdd & add )
{
    if ( _client == NULL || ! _client->isAgent() )
        return;

    LogFacility::LogMessage("TnmsMessageHandler::AddHandler() " 
        + add.getElementName());

    _tree->add(add.getElementName());

    return;
}


void  
TnmsMessageHandler::RemoveHandler ( const TnmsRemove  & remove )
{
    if ( _client == NULL || ! _client->isAgent() )
        return;
    
    LogFacility::LogMessage("TnmsMessageHandler::RemoveHandler() " 
        + remove.getElementName());

    _tree->remove(remove.getElementName());

    return;
}


void
TnmsMessageHandler::MetricHandler ( const TnmsMetric & metric )
{
    if ( _client == NULL || ! _client->isAgent() )
        return;

    LogFacility::LogMessage("TnmsMessageHandler::MetricHandler() " 
        + metric.getElementName());

    if ( _client->inTreeSend() )
        this->LastMessageHandler(ADD_MESSAGE);

    if ( metric.getElementName().empty() ) // || > max strlength
        return;

    _tree->update(metric);

    return;
}


void  
TnmsMessageHandler::RequestHandler ( const TnmsRequest & request )
{
    if ( _client == NULL || ! _client->isAuthorized() ) 
        return;

    TnmsMetric  metric;

    LogFacility::LogMessage("TnmsMessageHandler::RequestHandler() " 
        + request.getElementName());

    if ( _tree->request(request.getElementName(), metric) )
        _client->sendMessage(&metric);

    return;
}

void  
TnmsMessageHandler::SubscribeHandler   ( const std::string & name )
{
    if ( _client == NULL ||  ! _client->isAuthorized() )
        return;

    LogFacility::LogMessage("TnmsMessageHandler::SubscribeHandler() " 
        + name);

    if ( ! _tree->subscribe(name, _client) )
        _client->unsubscribe(name);

    return;
}


void 
TnmsMessageHandler::UnsubscribeHandler ( const std::string & name )
{
    if ( _client == NULL ) 
        return;

    LogFacility::LogMessage("TnmsMessageHandler::UnsubscribeHandler() " 
        + name);

    _tree->unsubscribe(name, _client);
}


void  
TnmsMessageHandler::StructureHandler ( bool  subscribe )
{
    if ( _client == NULL ||  ! _client->isAuthorized() )
        return;

    LogFacility::LogMessage("TnmsMessageHandler::StructureHandler()");

    if ( subscribe )
        _tree->subStructure(_client);
    else
        _tree->unsubStructure(_client);

    return;
}


void  
TnmsMessageHandler::AuthRequestHandler ( const TnmsAuthRequest & request )
{
    LogFacility::LogMessage("TnmsMessageHandler::AuthRequestHandler() " + request.getElementName());
    if ( _client ) 
        _client->AuthRequestHandler(request);
}


void  
TnmsMessageHandler::AuthReplyHandler   ( const TnmsAuthReply & reply )
{

    LogFacility::LogMessage("TnmsMessageHandler::AuthReplyHandler() " + reply.getElementName());
    if ( _client )
        _client->AuthReplyHandler(reply);
    return;
}


void  
TnmsMessageHandler::PingHandler() 
{
    LogFacility::LogMessage("TnmsMessageHandler::PingHandler() ");
}


void 
TnmsMessageHandler::PingReplyHandler()
{
    LogFacility::LogMessage("TnmsMessageHandler::PingReplyHandler() ");
}


void  
TnmsMessageHandler::LastMessageHandler ( int record_type ) 
{
    if ( _client == NULL ) 
        return;

    LogFacility::LogMessage("TnmsMessageHandler::LastMessageHandler()");

    if ( _client->inTreeSend() && 
         (record_type == ADD_MESSAGE || record_type == METRIC_MESSAGE) )
        _client->inTreeSend(false);

    return;
}


} // namespace

// _TNMSD_TNMSMESSAGEHANDLER_CPP_

