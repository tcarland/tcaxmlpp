#define _TNMSD_TNMSMESSAGEHANDLER_CPP_

#include "TnmsMessageHandler.h"
#include "TnmsClient.h"
#include "TnmsTree.h"
#include "TnmsSubscriber.h"

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
    if ( _client == NULL || ! _client->isAuthorized() )
        return;

    // if ( ! _client->isAgent() ) return;

    if ( _tree == NULL )
        return;

    //if ( LogFacility::GetDebug() )
        //LogFacility::LogMessage("TnmsMessageHandler::AddHandler() " 
            //+ add.getElementName());

    _tree->add(add.getElementName());

    return;
}


void  
TnmsMessageHandler::RemoveHandler ( const TnmsRemove  & remove )
{
    if ( _client == NULL || ! _client->isAuthorized() )
        return;
    
    // if ( ! _client->isAgent() ) return;

    if ( _tree == NULL )
        return;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsMessageHandler::RemoveHandler() "
            + remove.getElementName());

    _tree->remove(remove.getElementName());

    return;
}


void
TnmsMessageHandler::MetricHandler ( const TnmsMetric & metric )
{
    if ( _client == NULL || ! _client->isAuthorized() )
        return;

    //if ( ! _client->isAgent() ) return;

    //if ( LogFacility::GetDebug() )
        //LogFacility::LogMessage("TnmsMessageHandler::MetricHandler() " 
            //+ metric.getElementName());

    if ( _tree == NULL )
        return;

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

    if ( _tree == NULL )
        return;

    TnmsMetric  metric;

    if ( LogFacility::GetDebug() ) {
        LogFacility::LogMessage("TnmsMessageHandler::RequestHandler() " 
            + request.getElementName());
    }

    _tree->request(request.getElementName(), (TreeSubscriber*)_client->getSubscriber());

    return;
}

void  
TnmsMessageHandler::SubscribeHandler   ( const std::string & name )
{
    if ( _client == NULL ||  ! _client->isAuthorized() )
        return;

    if ( _tree == NULL )
        return;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsMessageHandler::SubscribeHandler() " + name);

    if ( ! _tree->subscribe(name, (TreeSubscriber*)_client->getSubscriber()) )
        _client->unsubscribe(name);

    return;
}


void 
TnmsMessageHandler::UnsubscribeHandler ( const std::string & name )
{
    if ( _client == NULL ) 
        return;

    if ( _tree == NULL )
        return;

    //if ( LogFacility::GetDebug() )
        //LogFacility::LogMessage("TnmsMessageHandler::UnsubscribeHandler() " + name);

    _tree->unsubscribe(name, (TreeSubscriber*)_client->getSubscriber());

    return;
}


void  
TnmsMessageHandler::StructureHandler ( bool  subscribe )
{
    if ( _client == NULL ||  ! _client->isAuthorized() )
        return;

    if ( _tree == NULL )
        return;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsMessageHandler::StructureHandler()");

    if ( subscribe )
        _tree->subStructure((TreeSubscriber*)_client->getSubscriber());
    else
        _tree->unsubStructure((TreeSubscriber*)_client->getSubscriber());

    return;
}


void  
TnmsMessageHandler::AuthRequestHandler ( const TnmsAuthRequest & request )
{
    if ( LogFacility::GetDebug() ) {
        LogFacility::LogMessage("TnmsMessageHandler::AuthRequestHandler() " 
            + request.getElementName());
    }

    if ( _client ) 
        _client->AuthRequestHandler(request);

    return;
}


void  
TnmsMessageHandler::AuthReplyHandler   ( const TnmsAuthReply & reply )
{
    if ( LogFacility::GetDebug() ) {
        LogFacility::LogMessage("TnmsMessageHandler::AuthReplyHandler() " 
            + reply.getElementName());
    }

    if ( _client )
        _client->AuthReplyHandler(reply);

    return;
}


void  
TnmsMessageHandler::PingHandler() 
{
    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsMessageHandler::PingHandler() ");

    if ( _client == NULL )
        return;
    
    TnmsMessage ping(PING_REPLY);
    _client->sendMessage(&ping, true);

    return;
}


void 
TnmsMessageHandler::PingReplyHandler()
{
    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsMessageHandler::PingReplyHandler() ");
    return;
}


void  
TnmsMessageHandler::LastMessageHandler ( int record_type ) 
{
    if ( _client == NULL ) 
        return;

    if ( _client->inTreeSend() && 
         (record_type == ADD_MESSAGE || record_type == METRIC_MESSAGE) )
    {
        _client->inTreeSend(false);
        if ( LogFacility::GetDebug() )
            LogFacility::LogMessage("TnmsMessageHandler::LastMessageHandler() tree received");
    }

    return;
}


} // namespace

// _TNMSD_TNMSMESSAGEHANDLER_CPP_

