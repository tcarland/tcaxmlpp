#define _TNMSDB_ARCHIVERTHREAD_CPP_

#include "ArchiverThread.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsdb {


ArchiverThread::ArchiverThread ( SqlSession * sql, SchemaConfigList & config )
    : _sql(sql),
      _upqueue(SynchronizedQueue<TnmsMetric>(ARCHIVE_QUEUESIZE)),
      _remqueue(SynchronizedQueue<std::string>(ARCHIVE_QUEUESIZE)),
      _lock(new ThreadLock())
{
    this->init(sql, config);    
}


ArchiverThread::~ArchiverThread()
{
    ArchiverSet::iterator aIter;
    for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
    {
        Archiver * archiver = (Archiver*) *aIter;
        if ( archiver )
            delete archiver;
    }
    _archivers.clear();
    delete _lock;
}


void
ArchiverThread::init ( SqlSession * sql, SchemaConfigList & config )
{
    SchemaConfigList::iterator  sIter;

    for ( sIter = config.begin(); sIter != config.end(); ++sIter )
    {
        Archiver * archiver = new Archiver(sql, *sIter);
        _archivers.push_back(archive);
    }

    return;
}


void
ArchiverThread::run()
{
    LogFacility::LogMessage("ArchiverThread started");

    while ( ! this->_Alarm )
    {
        while ( _upqueue.empty() && _remqueue.empty() )
        {
            _mutex.lock();
            _mutex.waitFor(5);
            _mutex.unlock();
        }

        while ( ! _upqueue.empty() )
        {
            if ( _upqueue.pop(metric) )
                this->updateWriters(metric);
        }

        while ( ! _remqueue.empty() )
        {
            if ( _remqueue.pop(name) )
                this->updateWriters(name);
        }

        time_t  now = LogFacility::GetLogTime();

        ArchiverSet::iterator aIter;
        for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
        {
            Archiver * archiver = (Archiver*) *aIter;

            // check for archiver timer, write updates to db

        }
    }
    LogFacility::LogMessage("ArchiverThread finished.");

    return;
}

void
ArchiverThread::notify()
{
    this->_lock->notify();
}


bool
ArchiverThread::queueUpdate ( const TnmsMetric & metric )
{
    if ( ! _upqueue.push(metric) )
        return false;
    return true;
}

bool
ArchiverThread::queueRemove ( const std::string & name )
{
    if ( ! _remqueue.push(name) )
        return false;
    return true;
}


void
ArchiverThread::updateWriters ( const TnmsMetric & metric )
{
    ArchiverSet::iterator aIter;
    for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
    {
        Archiver * archiver = (Archiver*) *aIter;
        archiver->update(metric);
    }
}


void
ArchiverThread::updateWriters ( const std::string & name )
{
    ArchiverSet::iterator aIter;
    for ( aIter = _archivers.begin(); aIter != _archivers.end(); ++aIter )
    {
        Archiver * archiver = (Archiver*) *aIter;
        archiver->remove(name);
    }
}




}  // namespace 


// _TNMSDB_ARCHIVERTHREAD_CPP_

