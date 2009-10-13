#ifndef _TNMSDB_ARCHIVER_H_
#define _TNMSDB_ARCHIVER_H_

#include <string>

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;


#include "ArchiveConfig.h"



namespace tnmsdb {



class Archiver {

  public:

    Archiver ( SqlSession * session, SchemaConfig & config );

    virtual ~Archiver();


    
  public:

    SqlSession *        sql;
    TnmsTree *          tree;
    SchemaConfig        config;
};


}  // namespace

#endif  // _TNMSDB_ARCHIVER_H_

