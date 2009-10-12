#ifndef _TNMSDB_ARCHIVER_H_
#define _TNMSDB_ARCHIVER_H_

#include <string>

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;


#include "ArchiveDbMaintainer.h"
#include "ArchiverConfig.h"



namespace tnmsdb {


class ArchiveClient;


class Archiver : public ArchiveDbMaintainer {

  public:

    Archiver ( SqlSession * session, SchemaConfig & config );

    virtual ~Archiver();


    void     update  ( const TnmsMetric  & metric );
    void     remove  ( const std::string & name );

    
  public:

    SqlSession *        _sql;
    TnmsTree *          _tree;
    SchemaConfig        _config;
};


}  // namespace

#endif  // _TNMSDB_ARCHIVER_H_

