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

/** Initiates the start of the thread by create the underlying thread object.
 *  If the underlying call to the thread create() fails, a ThreadException is 
 *  thrown. Once start() succeeds, the new thread will initiate with the private 
 *  threadEntry function.
 **/
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

/**  Forcibly stops the thread, and if not detached from the thread it will
 *   attempt to join the thread. A ThreadException is thrown if the join 
 *   fails.
 **/
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


/**  Sets a thread identity or name.  The thread will automatically be given 
 *   a name consisting of "Thread_xxx" where xxx is the thread id.  This
 *   method overrides that default name
 **/
void
Thread::threadName ( const std::string & name )
{
    _threadName = name;
}


/**  Returns the thread name or string identifier of the thread */
const std::string&
Thread::threadName() const
{
    return this->_threadName;
}


/**  Indicates whether the thread is currently in the run state. */
bool
Thread::isRunning()
{
    return this->_running;
}

/**  Sets the _Alarm boolean to true. This has no other behavior and 
 *   is nothing but a convenience state flag for the derived object 
 *   to utilize in its 'run()' implementation.
 **/
void
Thread::setAlarm()
{
    _Alarm = true;
}


/**  The private, thread entry function used as the entry point for 
 *   all new threads.
 **/
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

/**  Yields the thread by calling the pthread 'sched_yield()' function. This 
 *   method is protected and may only be called internally by the derived 
 *   object.
 **/
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
