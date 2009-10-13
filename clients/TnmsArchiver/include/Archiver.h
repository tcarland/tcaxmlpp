#ifndef _TNMSDB_ARCHIVER_H_
#define _TNMSDB_ARCHIVER_H_

#include <string>

#include "ArchiveDbMaintainer.h"
#include "ArchiveConfig.h"

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;

#include "SqlSession.hpp"
using namespace tcasqlpp;



namespace tnmsdb {

class ArchiveSubscriber;


class Archiver : public ArchiveDbMaintainer {

  public:

    Archiver ( SqlSession * session, SchemaConfig & config );

    virtual ~Archiver();

    void  runUpdates ( const time_t & now, bool flush );
    

  public:

    SqlSession *        sql;
    TnmsTree *          tree;
    ArchiveSubscriber*  notifier;
    SchemaConfig        schema;
};


}  // namespace

#endif  // _TNMSDB_ARCHIVER_H_

