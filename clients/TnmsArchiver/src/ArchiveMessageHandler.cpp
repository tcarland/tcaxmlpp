#define _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_CPP_


#include "ArchiveMessageHandler.h"

#include "ArchiverThread.h"



namespace tnmsarchive {


ArchiveMessageHandler::ArchiveMessageHandler ( const std::string & root_name, ArchiveClient * client, 
                                               ArchiverThread * archiver )
    : _client(client),
      _archiver(archiver),
      _rootname(root_name)
{}


ArchiveMessageHandler::~ArchiveMessageHandler() {}


void
ArchiveMessageHandler::AddHandler ( const TnmsAdd & add )
{
    ArchiverSet::iterator  aIter;
     
    if ( ! StringUtils::startswith(add.getElementName(), _rootname) )
        return;
    else
        _client->subscribe(add.getElementName());

    TnmsMetric  metric(add.getElementName());
    _archiver->queueUpdate(metric);

    return;
}


void
ArchiveMessageHandler::RemoveHandler ( const TnmsRemove  & remove )
{
    ArchiverSet::iterator  aIter;

    if ( _rootname.compare(remove.getElementName()) == 0 )
        client->setSubscribed(false);
        
    _archiver->queueRemove(remove.getElementName());

    return;
}


void
ArchiveMessageHandler::MetricHandler ( const TnmsMetric  & metric )
{
    ArchiverSet::iterator  aIter;

    if ( ! StringUtils::startsWith(metric.getElementName(), _rootname) )
        return;

    _archiver->queueUpdate(metric);

    return;
}


void
ArchiveMessageHandler::RequestHandler ( const TnmsRequest & request )
{}


void
ArchiveMessageHandler::SubscribeHandler ( const std::string & name )
{}


void
ArchiveMessageHandler::UnsubscribeHandler ( const std::string & name )
{
    if ( _rootname.compare(name) == 0 )
        _client->setSubscribed(false);
}


void
ArchiveMessageHandler::StructureHandler ( bool  subscribe )
{}


void
ArchiveMessageHandler::AuthReplyHandler ( const TnmsAuthReply   & reply )
{
    _client->AuthReplyHandler(reply);    
}


void
ArchiveMessageHandler::LastMessageHandler ( int   record_type )
{}


void
ArchiveMessageHandler::PingHandler()
{}


}  // namespace 


// _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_CPP_

