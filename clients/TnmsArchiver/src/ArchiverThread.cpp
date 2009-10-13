#define _TNMSDB_ARCHIVERTHREAD_CPP_

#include "ArchiverThread.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsdb {


ArchiverThread::ArchiveTimer::ArchiveTimer()
    : mutex(new ThreadLock()),
      id(0)
{
}

ArchiverThread::ArchiveTimer::~ArchiveTimer()
{
    delete mutex;
}

void
ArchiverThread::ArchiveTimer::timeout ( const EventTimer * timer )
{
    mutex->notify();
}


ArchiverThread::ArchiverThread ( EventManager * evmgr, SqlSession * sql, SchemaConfigList & config )
    : tree(new TnmsTree()),
      _sql(sql),
      _archiver(new Archiver(sql, config)),
      _tid(0),
      _fid(0)
{
    _tid = evmgr->addTimerEvent(&_timer, config.commit_interval_s, 0);
    // if ( _tid == 0 ) throw Exception("Failed to add timer event to EventManager");
    if ( config.flush_interval > 0 )
        _fid = evmgr->addTimerEvent(&_timer, (config.commit_interval_s * config.flush_interval), 0);
}


ArchiverThread::~ArchiverThread()
{
    delete tree;
}


void
ArchiverThread::run()
{
    LogFacility::LogMessage("ArchiverThread started");

    while ( ! this->_Alarm )
    {
        this->timer->mutex->lock();
        this->timer->mutex->wait();

        time_t  now = LogFacility::GetLogTime();

        // run updates
        if ( timer.id == _tid )
            this->runUpdates();
        else if ( timer.id == _fid )
            this->runUpdates(true);
    }

    LogFacility::LogMessage("ArchiverThread finished.");

    return;
}

void
ArchiverThread::notify()
{
    this->timer.mutex->notify();
}


void
ArchiverThread::runUpdates ( bool flush )
{
    tree->updateSubscribers();
    _archive->runUpdates(flush)
}



}  // namespace 


// _TNMSDB_ARCHIVERTHREAD_CPP_

