#ifndef _TNMSARCHIVE_ARCHIVERTHREAD_H_
#define _TNMSARCHIVE_ARCHIVERTHREAD_H_

#include <set>

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;


#include "Thread.h"
#include "ThreadLock.h"
#include "SynchronizedQueue.hpp"
using namespace tcanetpp;


namespace tnmsdb {


class ArchiveSubscriber;
class Archiver;

typedef std:set< Archiver* > ArchiverSet;



class ArchiverThread : public tcanetpp::Thread {

  public:

    ArchiverThread ( EventManager * evmgr, SqlSession * sql, SchemaConfigList & config );

    virtual ~ArchiverThread();

    
    void        run();
    void        notify();

  protected:

    void        init ( SqlSession * sql );

  private:

    EventManager *                      _evmgr;
    ArchiverSet                         _archivers;
    SynchronizedQueue<TnmsMetric>       _queue;
    ThreadLock *                        _lock;
};


} // namespace

#endif  // _TNMSARCHIVE_ARCHIVERTHREAD_H_

