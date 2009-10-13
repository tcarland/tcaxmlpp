#define _TNMSDB_ARCHIVER_CPP_

#include "Archiver.h"

#include "ArchiveClient.h"
#include "ArchiveSubscriber.h"


namespace tnmsdb {


Archiver::Archiver ( SqlSession * session, SchemaConfig & config )
    : ArchiveDbMaintainer(config.index_table, config.data_table, 1, config.tables_to_keep),
      sql(new SqlSession(*session)),
      tree(new TnmsTree()),
      notifier(new ArchiveSubscriber()),
      schema(config)
{
    tree->subscribe("*", notifier);
}


Archiver::~Archiver()
{
    delete tree;
}


void
Archiver::runUpdates ( const time_t & now, bool flush )
{
}


}  // namespace 

//  _TNMSDB_ARCHIVER_CPP_

