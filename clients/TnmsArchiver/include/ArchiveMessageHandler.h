#ifndef _TNMSDB_ARCHIVEMESSAGEHANDLER_H_
#define _TNMSDB_ARCHIVEMESSAGEHANDLER_H_

#include "MessageHandler.hpp"
using namespace tnmsCore;

#include "TnmsArchiveManager.h"


namespace tnmsdb {


class ArchiveClient;
class ArchiveThread;
class Archiver;


class ArchiveMessageHandler : public MessageHandler {

  public:

    ArchiveMessageHandler ( const std::string & root_name, ArchiveClient * client,
                            ArchiverSet & archivers );

    virtual ~ArchiveMessageHandler();


    virtual void  AddHandler         ( const TnmsAdd     & add );
    virtual void  RemoveHandler      ( const TnmsRemove  & remove );
    virtual void  MetricHandler      ( const TnmsMetric  & metric );
    virtual void  RequestHandler     ( const TnmsRequest & request );

    virtual void  SubscribeHandler   ( const std::string & name );
    virtual void  UnsubscribeHandler ( const std::string & name );
    virtual void  StructureHandler   ( bool  subscribe );

    virtual void  AuthRequestHandler ( const TnmsAuthRequest & request ) {}
    virtual void  AuthReplyHandler   ( const TnmsAuthReply   & reply );
    
    virtual void  LastMessageHandler ( int   record_type );

    virtual void  PingHandler();
    virtual void  PingReplyHandler() {}


  protected:

    ArchiveClient  *    _client;
    ArchiverSet         _archivers;
    std::string         _rootname;
};


}  // namespace


#endif //  _TNMSDB_ARCHIVEMESSAGEHANDLER_H_

