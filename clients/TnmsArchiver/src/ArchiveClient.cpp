#define _TNMSDB_ARCHIVECLIENT_CPP_


#include "ArchiveClient.h"


namespace tnmsdb {


ArchiveClient::ArchiveClient ( ArchiveDbConfig & config ) 
    : _config(config)
{
    this->init();
}

ArchiveClient::~ArchiveClient()
{
    ArchiveSet::iterator  aIter;

    for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
    {
        Archiver * archiver = (Archiver*) *aIter;
        if ( archiver )
            delete archiver;
    }
    _archivers.clear();
}


void
ArchiveClient::init()
{
    ArchiveDbConfig::iterator cIter;

    for ( cIter = _config.begin(); cIter != _config.end(); ++cIter )
    {
        Archiver * writer = new Archiver(*cIter);
        _archivers.push_back(writer);
    }
    this->setMessageHandler(new ArchiveMessageHandler(_archivers));
}

void
ArchiveClient::setSubscribed ( bool sub )
{
    _subscribed = sub;
}


}  // namespace


