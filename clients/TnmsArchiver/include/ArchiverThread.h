#ifndef _TNMSDB_ARCHIVERTHREAD_H_
#define _TNMSDB_ARCHIVERTHREAD_H_

#include <set>

#include "ArchiveConfig.h"

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;

#include "EventManager.h"
#include "EventHandlers.hpp"
#include "Thread.h"
#include "ThreadLock.h"
using namespace tcanetpp;

#include "SqlSession.hpp"
using namespace tcasqlpp;


namespace tnmsdb {


class DbArchiver;



class ArchiverThread : public tcanetpp::Thread {

  public:

    ArchiverThread ( EventManager   * evmgr,
                     SqlSession     * sql, 
                     SchemaConfig   & configlist );

    virtual ~ArchiverThread();


    void        run();

    void        notify();


  protected:

    void        runUpdates ( const time_t & now, bool flush = false );

   
  public:

    class ArchiveTimer : public EventTimerHandler {

      public:
        ArchiveTimer();
        virtual ~ArchiveTimer();

        virtual void timeout  ( const EventTimer & timer );
        virtual void finished ( const EventTimer & timer ) {}

      public:

        ThreadLock *    mutex;
        evid_t          id;
    };
        
    TnmsTree *          tree;


  private:

    DbArchiver *        _archiver;
    ArchiveTimer        _timer;


    evid_t              _tid, _fid;

};


} // namespace

#endif  // _TNMSDB_ARCHIVERTHREAD_H_

