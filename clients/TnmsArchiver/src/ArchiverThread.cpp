#define _TNMSDB_ARCHIVERTHREAD_CPP_

#include "ArchiverThread.h"
#include "Archiver.h"

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

// -------------------------------------------------------------------

ArchiverThread::ArchiverThread ( EventManager * evmgr, SqlSession * sql, SchemaConfig & config )
    : _archiver(new Archiver(sql, config)),
      _tid(0),
      _fid(0)
{
    _tid = evmgr->addTimerEvent(&_timer, config.commit_interval_s, 0);
    // if ( _tid == 0 ) throw Exception("Failed to add timer event to EventManager");
    if ( config.flush_interval > 0 )
        _fid = evmgr->addTimerEvent(&_timer, (config.commit_interval_s * config.flush_interval), 0);
    this->tree = _archiver->tree;
}


ArchiverThread::~ArchiverThread()
{
    delete _archiver;
}


void
ArchiverThread::run()
{
    LogFacility::LogMessage("ArchiverThread started");

    while ( ! this->_Alarm )
    {
        this->_timer.mutex->lock();
        this->_timer.mutex->wait();

        time_t  now = LogFacility::GetLogTime();

        // run updates
        if ( _timer.id == _tid )
            this->runUpdates(now);
        else if ( _timer.id == _fid )
            this->runUpdates(now, true);

        this->_timer.mutex->unlock();
    }

    LogFacility::LogMessage("ArchiverThread finished.");

    return;
}

void
ArchiverThread::notify()
{
    this->_timer.mutex->notify();
}


void
ArchiverThread::runUpdates ( const time_t & now, bool flush )
{
    _timer.id = 0;
    tree->updateSubscribers();
    _archiver->runUpdates(now, flush);
}



}  // namespace 


// _TNMSDB_ARCHIVERTHREAD_CPP_

