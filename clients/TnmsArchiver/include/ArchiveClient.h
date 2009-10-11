#ifndef _TNMSDB_ARCHIVECLIENT_H_
#define _TNMSDB_ARCHIVECLIENT_H_

#include "TnmsClient.h"

#include "ArchiverConfig.h"


namespace tnmsdb {


class ArchiveClient : public tnmsCore::TnmsClient {

  public:

    ArchiveClient ( const std::string & name, ArchiverSet & archivers );
    virtual ~ArchiveClient();

    void     init();

    void     setSubscribed ( bool sub );

  protected:

    ArchiverSet &    _archivers;
    
    std::string      _name;

};

}  // namespace


#endif  // _TNMSDB_ARCHIVECLIENT_H_

