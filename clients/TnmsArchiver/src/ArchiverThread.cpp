#define _TNMSARCHIVE_ARCHIVERTHREAD_CPP_


#include "ArchiverThread.h"


namespace tnmsArchive {


ArchiverThread::ArchiverThread ( EventManager * evmgr, SqlSession * sql, SchemaConfigList & config )
    : _evmgr(evmgr),
      _sql(sql),
      _lock(new ThreadLock())
{}


ArchiverThread::~ArchiverThread()
{
    delete _lock;
}


void
ArchiverThread::init()
{}


void
ArchiverThread::run()
{

}

void
ArchiverThread::notify()
{

}



}  // namespace 


// _TNMSARCHIVE_ARCHIVERTHREAD_CPP_

