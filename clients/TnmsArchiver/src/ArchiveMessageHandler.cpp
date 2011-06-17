#define _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_CPP_

#include "ArchiveMessageHandler.h"

#include "ArchiverThread.h"
#include "ArchiveClient.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsdb {


ArchiveMessageHandler::ArchiveMessageHandler ( const std::string & root_name, 
                                               ArchiveClient     * client, 
                                               ArchiverSet       & archivers )
    : _client(client),
      _archivers(archivers),
      _rootname(root_name)
{}


ArchiveMessageHandler::~ArchiveMessageHandler() {}


void
ArchiveMessageHandler::AddHandler ( const TnmsAdd & add )
{
    ArchiverSet::iterator  aIter;
    const std::string    & name = add.getElementName();
     
    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("ArchiveMessageHandler::AddHandler() " + name);

    if ( StringUtils::startsWith(name, _rootname) )
        _client->subscribe(name);
    else {
        LogFacility::LogMessage("ArchiveMessageHandler::AddHandler() ignoring "
                + name);
        return;
    }

    for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
        (*aIter)->tree->add(name);

    return;
}


void
ArchiveMessageHandler::RemoveHandler ( const TnmsRemove & remove )
{
    ArchiverSet::iterator  aIter;
    const std::string    & name = remove.getElementName();

    if ( _rootname.compare(name) == 0 )
        _client->setSubscribed(false);
        
    for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
        (*aIter)->tree->remove(name);

    return;
}


void
ArchiveMessageHandler::MetricHandler ( const TnmsMetric & metric )
{
    ArchiverSet::iterator  aIter;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("ArchiveMessageHandler::MetricHandler() " 
            + metric.getElementName());

    if ( ! StringUtils::startsWith(metric.getElementName(), _rootname) ) {
        LogFacility::LogMessage("ArchiveMessageHandler::MetricHandler() ignoring "
                + metric.getElementName());
        return;
    }

    for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
        (*aIter)->tree->update(metric);

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
ArchiveMessageHandler::AuthReplyHandler ( const TnmsAuthReply & reply )
{
    _client->AuthReplyHandler(reply);    
}


void
ArchiveMessageHandler::LastMessageHandler ( int  record_type )
{}


void
ArchiveMessageHandler::PingHandler()
{}


}  // namespace 


// _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_CPP_

