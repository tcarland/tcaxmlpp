#define _TNMSD_TNMSMESSAGEHANDLER_CPP_

#include "TnmsMessageHandler.h"

namespace tnmsd {

TnmsMessageHandler::TnmsMessageHandler ( TnmsTree * tree, TnmsClient * client )
    : _tree(tree), 
      _client(client),
      _isapi(false) 
{}

TnmsMessageHandler::~TnmsMessageHandler()
{}


void
TnmsMessageHandler::AddHandler ( const TnmsAdd & add )
{
    if ( ! _client->isAPI() )
        return;

    _tree->add(add.getElementName());
    return;
}


void  
TnmsMessageHandler::RemoveHandler ( const TnmsRemove  & remove )
{
    if ( ! _client->isAPI() )
        return;

    _tree->remove(remove.getElementName());
    return;
}


void
TnmsMessageHandler::MetricHandler ( const TnmsMetric & metric )
{
    if ( ! _client->isAPI() )
        return;

    if ( client->inTreeSend() )
        this->LastMessageHandler(ADD_MESSAGE);

    if ( metric->getElementName().empty() ) // || > max strlength
        return;

    _tree->update(metric);
    return;
}


void  
TnmsMessageHandler::RequestHandler ( const TnmsRequest & request )
{
    TnmsMetric  metric;

    if ( _tree->request(request.getElementName(), metric) )
        _client->queueUpdate(metric);

    return;
}

void  
TnmsMessageHandler::SubscribeHandler   ( const std::string & name )
{
    //if ( ! _client->authorized() )
    //    return;

    if ( ! _tree->subscribe(name, _client) )
        _client->unsubscribe(name);

    return;
}


void 
TnmsMessageHandler::UnsubscribeHandler ( const std::string & name )
{
    _tree->unsubscribe(name, _client);
}


void  
TnmsMessageHandler::StructureHandler ( bool  subscribe )
{
    // if not authorized; return;

    if ( subscribe )
        _tree->subStructure(_client);
    else
        _tree->unsubStructure(_client);

    return;
}


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
TnmsMessageHandler::LastMessageHandler ( int record_type ) 
{
    if ( _client->inTreeSend() && (type == ADD_MESSAGE || type == METRIC_MESSAGE) )
        _client->inTreeSend(false);

    return;
}


} // namespace

// _TNMSD_TNMSMESSAGEHANDLER_CPP_

