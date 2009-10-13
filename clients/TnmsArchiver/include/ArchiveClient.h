#ifndef _TNMSDB_ARCHIVECLIENT_H_
#define _TNMSDB_ARCHIVECLIENT_H_

#include "TnmsClient.h"

#include "ArchiverConfig.h"


namespace tnmsdb {

class ArchiverThread;


class ArchiveClient : public tnmsCore::TnmsClient {

  public:

    ArchiveClient ( ArchiverThread * archiver, TnmsClientConfig & cfg );

    virtual ~ArchiveClient();


    void     init();
    void     setSubscribed ( bool sub );


  protected:
    
    ArchiverThread *   _archiver;
    
    TnmsClientConfig   _cfg;

};

}  // namespace


#endif  // _TNMSDB_ARCHIVECLIENT_H_

