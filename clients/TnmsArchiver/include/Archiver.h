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

typedef std::map<std::string, uint32_t>    IndexMap;
typedef std::map<std::string, TnmsMetric>  ElementMap;


class Archiver : public ArchiveDbMaintainer {

  public:

    Archiver ( SqlSession * sql_, SchemaConfig & config );

    virtual ~Archiver();


    void      runUpdates ( const time_t & now, bool flush );
   

  protected:

    void      loadIndexMap();

    uint32_t  getElementId    ( const std::string & name );
    uint32_t  queryElementId  ( const std::string & name );
    uint32_t  insertElementId ( const std::string & name );


  public:

    SqlSession *        sql;
    TnmsTree *          tree;
    ArchiveSubscriber * notifier;

    SchemaConfig        schema;


  protected:

    IndexMap    _indexes;
    ElementMap  _elements;

};


}  // namespace

#endif  // _TNMSDB_ARCHIVER_H_

