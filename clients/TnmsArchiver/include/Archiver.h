#ifndef _TNMSDB_ARCHIVER_H_
#define _TNMSDB_ARCHIVER_H_

#include <string>

#include "TnmsTree.h"
using namespace tnmsCore;

#include "ArchiverConfig.h"



namespace tnmsdb {

class ArchiveClient;


class Archiver : public EventTimerHandler {

  public:

    Archiver ( SchemaConfig & config, SqlSession * session );

    virtual ~Archiver();

    virtual void  timeout  ( const EventTimer * timer );
    virtual void  finished ( const EventTimer * timer ) {}


    
  public:

    SqlSession *        sql;
    TnmsTree *          tree;
    SchemaConfig        config;

};

}  // namespace

#endif  // _TNMSDB_ARCHIVER_H_

