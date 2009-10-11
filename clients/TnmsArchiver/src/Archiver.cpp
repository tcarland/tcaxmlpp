#define _TNMSDB_ARCHIVER_CPP_

#include "Archiver.h"
#include "ArchiveClient.h"


namespace tnmsdb {


Archiver::Archiver ( SchemaConfig & config_, ArchiveClient * client_ )
    : tree(new TnmsTree()),
      client(client_),
      config(config_)
{}


Archiver::~Archiver()
{
    delete _tree;
}


void
Archiver::timeout ( const EventTimer * timer )
{
}



}  // namespace 

//  _TNMSDB_ARCHIVER_CPP_

