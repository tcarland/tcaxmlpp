#ifndef _TNMSDB_ARCHIVERTHREAD_H_
#define _TNMSDB_ARCHIVERTHREAD_H_

#include <set>

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;


#include "Thread.h"
#include "ThreadLock.h"
#include "SynchronizedQueue.hpp"
using namespace tcanetpp;


#define ARCHIVE_QUEUESIZE  (65535 * 16)


namespace tnmsdb {


class ArchiveSubscriber;
class Archiver;

typedef std:set< Archiver* > ArchiverSet;



class ArchiverThread : public tcanetpp::Thread {

  public:

    ArchiverThread ( SqlSession       * sql, 
                     SchemaConfigList & configlist );

    virtual ~ArchiverThread();

    bool        queueUpdate ( const TnmsMetric  & metric );
    bool        queueRemove ( const std::string & name );

    void        run();
    void        notify();

  protected:

    void        init ( SqlSession * sql, SchemaConfigList & configlist );

    void        updateWriters ( const TnmsMetric  & metric );
    void        updateWriters ( const std::string & name );


  private:

    SqlSession *                        _sql;
    ArchiverSet                         _archivers;

    SynchronizedQueue<TnmsMetric>       _upqueue;
    SynchronizedQueue<std::string>      _remqueue;

    ThreadLock *                        _lock;
};


} // namespace

#endif  // _TNMSDB_ARCHIVERTHREAD_H_

