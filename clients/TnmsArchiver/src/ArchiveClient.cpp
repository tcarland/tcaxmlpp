#define _TNMSDB_ARCHIVECLIENT_CPP_


#include "ArchiveClient.h"
#include "ArchiverThread.h"


namespace tnmsdb {


ArchiveClient::ArchiveClient ( ArchiverThread * archiver, TnmsClientConfig & config ) 
    : _archiver(archiver),
      _config(config)
{
    this->init();
}

ArchiveClient::~ArchiveClient()
{
}


void
ArchiveClient::init()
{
    SubsList &  subs = _config.subs;

    if ( subs.empty() )
        return;

    std::string  rname = subs.front();

    this->setMessageHandler(new ArchiveMessageHandler(rname, _archiver));
}

void
ArchiveClient::setSubscribed ( bool sub )
{
    _subscribed = sub;
}


}  // namespace


