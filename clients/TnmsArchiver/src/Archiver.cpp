#define _TNMSDB_ARCHIVER_CPP_

#include "Archiver.h"
#include "ArchiveClient.h"


namespace tnmsdb {


Archiver::Archiver ( SqlSession * session, SchemaConfig & config )
    : ArchiverDbMaintainer(config.index_table, config.data_table, 1, config.tables_to_keep),
      _sql(new SqlSession(*session)),
      _tree(new TnmsTree()),
      _flushId(0),
      _config(config)
{
    _timerId = evmgr->addTimerEvent(&_timer, _config.commit_interval_s, 0);
    if ( _config.flush_interval > 0 )
        _flushId = evmgr->addTimerEvent(&_timer, (_config.commit_interval_s * _config.flush_interval), 0);
}


Archiver::~Archiver()
{
    delete _tree;
}


void
Archiver::init()
{

}


void
Archiver::update ( const TnmsMetric & metric )
{
    _tree->update(metric);
}

void
Archiver::remove ( const std::string & name )
{
    _tree->remove(name);
}


}  // namespace 

//  _TNMSDB_ARCHIVER_CPP_

