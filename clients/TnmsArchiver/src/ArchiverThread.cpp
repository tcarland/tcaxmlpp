#define _TNMSARCHIVE_ARCHIVERTHREAD_CPP_


#include "ArchiverThread.h"


namespace tnmsArchive {


ArchiverThread::ArchiverThread()
    : _tree(new TnmsTree()),
      _lock(new ThreadLock())
{}


ArchiverThread::~ArchiverThread()
{
    delete _lock;
    delete _tree;
}


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

