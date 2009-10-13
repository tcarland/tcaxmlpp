#ifndef _TNMSDB_ARCHIVERTHREAD_H_
#define _TNMSDB_ARCHIVERTHREAD_H_

#include <set>

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;

#include "EventManager.h"
#include "EventHandlers.hpp"
#include "Thread.h"
#include "ThreadLock.h"
using namespace tcanetpp;


#define ARCHIVE_QUEUESIZE  (65535 * 16)


namespace tnmsdb {


class ArchiveSubscriber;
class Archiver;

typedef std:set< Archiver* > ArchiverSet;



class ArchiverThread : public tcanetpp::Thread {

  public:

    ArchiverThread ( EventManager     * evmgr,
                     SqlSession       * sql, 
                     SchemaConfigList & configlist );

    virtual ~ArchiverThread();

    bool        queueUpdate ( const TnmsMetric  & metric );
    bool        queueRemove ( const std::string & name );

    void        run();
    void        notify();

  protected:

    void        runUpdates ( bool flush = false );

   
  public:

    class ArchiveTimer : public EventTimerHandler {

      public:
        ArchiveTimer();
        virtual ~ArchiveTimer();

        virtual void timeout  ( const EventTimer * timer );
        virtual void finished ( const EventTimer * timer ) {}

      public:

        ThreadLock *    mutex;
        evid_t          id;
    };
        
  public:
    
    TnmsTree *          tree;

  private:

    SqlSession *        _sql;
    Archiver *          _archiver;
    ArchiveTimer        _timer;

    evid_t              _tid, _fid;

};


} // namespace

#endif  // _TNMSDB_ARCHIVERTHREAD_H_

