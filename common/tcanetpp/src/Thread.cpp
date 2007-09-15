/**  Abstract Thread classe
  *
  *  @Author  tcarland@gmail.com
  *  @Version 1.8
 **/
#define _TCANETPP_THREAD_CPP_

extern "C" {
# include <strings.h>
# include <stdio.h>
# include <errno.h>
}

#include <sstream>

#include "Thread.h"


namespace tcanetpp {


static const
char rcsid[] = "$Id: Thread.cpp,v 1.8 2007/09/16 04:40:57 tca Exp $";


Thread::Thread()
    : _Alarm(false),
      _running(false),
      _detach(false),
      _tid(0)
{}


Thread::Thread ( bool detach )
    : _Alarm(false),
      _running(false),
      _detach(detach),
      _tid(0)
{}


Thread::~Thread()
{
    if ( _tid != 0 )
	this->stop();
}


void
Thread::start() throw ( ThreadException )
{
    if ( _tid != 0 )
	throw ThreadException("Thread::start() id non-zero, already started?");

    if ( pthread_create(&_tid, NULL, threadEntry, (void*)this) != 0 ) {
        std::ostringstream  serr;
        serr << "Thread::start() pthread_create error: " << errno;
        _serr = serr.str();
	throw ThreadException(_serr);
    }

    _running = true;

    if ( _threadName.empty() ) {
        std::ostringstream  name;
        name << "Thread_" << _tid;
	_threadName = name.str();
    }

    if ( _detach )
        pthread_detach(_tid);

    return;
}


void
Thread::stop() throw ( ThreadException )
{
    if ( _tid == 0 )
	return;

    this->setAlarm();

    if ( ! _detach && pthread_join(_tid, NULL) != 0 ) {
        std::ostringstream  serr;
        serr << "Thread::stop() pthread_join error: "
             << _threadName << " : " << errno;
        _serr = serr.str();
	throw ThreadException(_serr);
    }

    _tid     = 0;
    _running = false;

    return;
}


void
Thread::threadName ( const std::string & name )
{
    _threadName = name;
}


const std::string&
Thread::threadName() const
{
    return this->_threadName;
}


bool
Thread::isRunning()
{
    return this->_running;
}


void
Thread::setAlarm()
{
    _Alarm = true;
}


void*
Thread::threadEntry ( void* arg )
{
    Thread * t = (Thread*) arg;

    if ( t == NULL )
        return NULL;

    t->init();
    t->run();
    t->finished();

    return((void*)t);
}


void
Thread::yield()
{
    sched_yield();
}


void
Thread::finished()
{
    this->_running = false;
}


} // namespace

//  _TCANETPP_THREAD_CPP_
