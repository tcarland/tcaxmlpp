#define _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_CPP_


#include "ArchiveMessageHandler.h"

#include "ArchiverThread.h"



namespace tnmsarchive {


ArchiveMessageHandler::ArchiveMessageHandler ( ArchiverThread * writer,
                                               TnmsTree       * tree,
                                               TnmsClient     * client )
    : TnmsMessageHandler(tree, client),
      _writer(writer)
{}


ArchiveMessageHandler::~ArchiveMessageHandler() {}


void
ArchiveMessageHandler::AddHandler ( const TnmsAdd & add )
{}

void
ArchiveMessageHandler::RemoveHandler ( const TnmsRemove  & remove )
{}
void
ArchiveMessageHandler::MetricHandler ( const TnmsMetric  & metric )
{}
void
ArchiveMessageHandler::RequestHandler ( const TnmsRequest & request )
{}
void
ArchiveMessageHandler::SubscribeHandler ( const std::string & name )
{}
void
ArchiveMessageHandler::UnsubscribeHandler ( const std::string & name )
{}
void
ArchiveMessageHandler::StructureHandler ( bool  subscribe )
{}
void
ArchiveMessageHandler::AuthRequestHandler ( const TnmsAuthRequest & request )
{}
void
ArchiveMessageHandler::AuthReplyHandler ( const TnmsAuthReply   & reply )
{}
void
ArchiveMessageHandler::LastMessageHandler ( int   record_type )
{}
void
ArchiveMessageHandler::PingHandler()
{}
void
ArchiveMessageHandler::PingReplyHandler()
{}



}  // namespace 


// _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_CPP_
