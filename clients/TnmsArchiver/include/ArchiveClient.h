#ifndef _TNMSDB_ARCHIVECLIENT_H_
#define _TNMSDB_ARCHIVECLIENT_H_

#include "TnmsClient.h"

#include "ArchiverConfig.h"


namespace tnmsdb {

class ArchiverThread;


class ArchiveClient : public tnmsCore::TnmsClient {

  public:

    ArchiveClient ( TnmsClientConfig & cfg, ArchiverThread * archiver );

    virtual ~ArchiveClient();


    void     init();
    void     setSubscribed ( bool sub );


  protected:
    
    TnmsClientConfig   _cfg;
    ArchiverThread *   _archiver;

};

}  // namespace


#endif  // _TNMSDB_ARCHIVECLIENT_H_

