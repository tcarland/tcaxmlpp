#ifndef _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_H_
#define _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_H_

#include "TnmsMessageHandler.h"
using namespace tnmsCore;


namespace tnmsarchive {


class ArchiverThread;


class ArchiveMessageHandler : public TnmsMessageHandler {

  public:

    ArchiveMessageHandler ( ArchiverThread * writer, 
                            TnmsTree       * tree, 
                            TnmsClient     * client );


    virtual ~ArchiveMessageHandler();

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

    WriterThread *      _writer;

};


}  // namespace


#endif //  _TNMSARCHIVE_ARCHIVEMESSAGEHANDLER_H_

