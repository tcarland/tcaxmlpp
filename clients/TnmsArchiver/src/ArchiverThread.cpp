#define _TNMSDB_ARCHIVERTHREAD_CPP_

#include "ArchiverThread.h"
#include "DbArchiver.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsdb {


// -------------------------------------------------------------------

void
ArchiverThread::ArchiveTimer::timeout ( const EventTimer & timer )
{
    this->id = timer.evid;

    if ( this->tree != NULL )
        this->tree->updateSubscribers();

    this->mutex.notify();
}

// -------------------------------------------------------------------


ArchiverThread::ArchiverThread ( EventManager * evmgr, 
                                 SqlSession   * sql, 
                                 SchemaConfig & config )
    : _archiver(new DbArchiver(sql, config)),
      _tid(0),
      _fid(0),
      _mid(0)
{
    this->tree        = _archiver->tree;
    this->_timer.tree = this->tree;

    _tid = evmgr->addTimerEvent(&_timer, config.commit_interval_s, 0);
    _mid = evmgr->addTimerEvent(&_mtimer, (3600 * 8), 0);
    
    if ( config.flush_interval > 0 )
        _fid = evmgr->addTimerEvent(&_timer, (config.commit_interval_s * config.flush_interval), 0);
}


ArchiverThread::~ArchiverThread()
{
    delete _archiver;
}


void
ArchiverThread::run()
{
    time_t  now = LogFacility::GetLogTime();

    LogFacility::LogMessage("ArchiverThread started");

    _archiver->runMaintainer(now);

    while ( ! this->_Alarm )
    {
        this->_timer.mutex.lock();
        this->_timer.mutex.wait();

        now = LogFacility::GetLogTime();

        if ( LogFacility::GetDebug() )
            LogFacility::LogMessage("ArchiverThread::run()");

        // run updates
        if ( _timer.id == _tid )
            this->runUpdates(now);
        else if ( _timer.id == _fid )
            this->runUpdates(now, true);
        else if ( _timer.id == _mid )
            _archiver->runMaintainer(now);

        this->_timer.mutex.unlock();
    }

    LogFacility::LogMessage("ArchiverThread finished.");

    return;
}

void
ArchiverThread::lock()
{
    this->_timer.mutex.lock();
}

void
ArchiverThread::unlock()
{
    this->_timer.mutex.unlock();
}

void
ArchiverThread::notify()
{
    this->_timer.mutex.notify();
}


void
ArchiverThread::runUpdates ( const time_t & now, bool flush )
{
    _timer.id = 0;
    _archiver->runUpdates(now, flush);
}



}  // namespace 


// _TNMSDB_ARCHIVERTHREAD_CPP_

