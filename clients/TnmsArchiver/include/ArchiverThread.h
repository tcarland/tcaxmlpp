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


class ArchiverThread : public tcanetpp::Thread {

  public:

    ArchiverThread ( SchemaConfig & config,  );

    virtual ~ArchiverThread();


    void        run();
    void        notify();


  private:

    tnmsCore::TnmsTree*           _tree;

    tcanetpp::ThreadLock*         _lock;
    ArchiveSubscriber *           _notifier;
};


} // namespace

#endif  // _TNMSARCHIVE_ARCHIVERTHREAD_H_

