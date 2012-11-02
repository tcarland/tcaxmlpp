/** 
  * @file Thread.cpp
  * 
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author  tcarland@gmail.com
  *
  * @section LICENSE
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
**/
#define _TCANETPP_THREAD_CPP_

extern "C" {
# include <strings.h>
# include <errno.h>
}

#include <cstdlib>
#include <sstream>

#include "Thread.h"


namespace tcanetpp {



/* -------------------------------------------------------------- */

/**  The thread constructor
  *  @param detach indicates whether the thread should be detached
  *  at initialization. Detaching a thread informs the OS to reclaim
  *  any resources used by the thread at thread termination.
 **/
Thread::Thread ( bool detach )
    : _Alarm(false),
      _running(false),
      _detach(detach),
      _tid(0),
      _stack(NULL)
{
    ::pthread_attr_init(&_attr);
}


Thread::~Thread()
{
    ::pthread_attr_destroy(&_attr);
    if ( _tid != 0 )
        this->stop();
    if ( _stack )
        ::free(_stack);
}

/* -------------------------------------------------------------- */
/** Initiates the start of the thread by creating the underlying thread object.
 *  If the underlying call to the thread create() fails, a ThreadException is 
 *  thrown. Once start() succeeds, the new thread will initiate via the  
 *  threadEntry function.
 **/
void
Thread::start() throw ( ThreadException )
{
    size_t stksz = 0;

    if ( _tid != 0 )
        throw ThreadException("Thread::start() id non-zero, already started?");

    if ( ! this->getStackSize(stksz) )
        throw ThreadException(_serr);

    if ( stksz < THREAD_STACKSIZE_MIN ) {
        stksz = THREAD_STACKSIZE_MIN;
        if ( ! this->setStackSize(stksz) )
            throw ThreadException(_serr);
    }

    if ( ::pthread_create(&_tid, &_attr, Thread::ThreadEntry, (void*)this) != 0 ) {
        std::ostringstream  serr;
        serr << "Thread::start() pthread_create error: " << errno;
        _serr = serr.str();
        throw ThreadException(_serr);
    }

    this->_running = true;

    if ( _threadName.empty() ) {
        std::ostringstream  name;
        name << "Thread_" << _tid; // pthread_self()
        _threadName = name.str();
    }

    if ( _detach )
        ::pthread_detach(_tid);

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

    if ( ! _detach && ::pthread_join(_tid, NULL) != 0 ) {
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

/* -------------------------------------------------------------- */

/**  Sets a thread identity or name.  The thread will automatically be given 
 *   a name consisting of "Thread_x" where x is the thread id.  This
 *   method overrides that default name.
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

/* -------------------------------------------------------------- */

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

/**  Yields the thread by calling the pthread 'sched_yield()' function. This
 *   method is protected and may only be called internally by the derived
 *   object.
 **/
void
Thread::yield()
{
    ::sched_yield();
}

/* -------------------------------------------------------------- */

/**  Sets the stack size of the Thread. This method should be 
  *  called prior to starting the Thread.
 **/
bool
Thread::setStackSize ( size_t stksz )
{
    if ( this->isRunning() ) {
        _serr  = "Thread is already running.";
        return false;
    }

    if ( stksz < THREAD_STACKSIZE_MIN )
        stksz = THREAD_STACKSIZE_MIN;

    _stack = ::malloc(stksz);
    if ( _stack == NULL ) {
        _serr  = "Error in malloc().";
        return false;
    }

    if ( ::pthread_attr_setstack(&_attr, _stack, stksz) ) {
        _serr = "Error in pthread_attr_setstack().";
        return false;
    }

    return true;
}

/**  Returns the Thread's configured stack size */
bool
Thread::getStackSize ( size_t & stksz )
{
    void * stack;

    if ( ::pthread_attr_getstack(&_attr, &stack, &stksz) ) {
        _serr = "Error in pthread_attr_getstack().";
        return false;
    }

    return true;
}

/* -------------------------------------------------------------- */

/**  Sets the CPU Affinity of the Thread object or essentially 
  *  which CPU should run this Thread. This can be called prior 
  *  to starting the Thread, or while it is running.
 **/
bool
Thread::setCpuAffinity ( long cpu )
{
    long maxcpus = Thread::MaxCPUs();
    int  err     = 0;

    if ( cpu < 0 || cpu > maxcpus ) {
        _serr = "Invalid CPU Affinity";
        return false;
    }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);

    if ( this->isRunning() )
        err = ::pthread_setaffinity_np(_tid, sizeof(mask), &mask);
    else
        err = ::pthread_attr_setaffinity_np(&_attr, sizeof(mask), &mask);

    if ( err ) {
        _serr = "Error setting thread CPU Affinity for " + _threadName;
        if ( err == EINVAL )
            _serr.append(" invalid parameter to pthread_setaffinity_np()");
        return false;
    }

    return true;
}

/**  Provides the current CPU Affinity of the Thread object by 
  *  setting the provided cpu_set_t.
 **/
bool
Thread::getCpuAffinity ( cpu_set_t & cpus )
{
    CPU_ZERO(&cpus);
    if ( ::pthread_getaffinity_np(_tid, sizeof(cpus), &cpus) )
        return false;
    return true;
}

/* -------------------------------------------------------------- */
//#if defined (_POSIX_THREAD_PRIORITY_SCHEDULING)

int
Thread::getMinPriority ( int policy )
{
    return(::sched_get_priority_min(policy));
}

int
Thread::getMaxPriority ( int policy )
{
    return(::sched_get_priority_max(policy));
}

/* -------------------------------------------------------------- */

/** Sets the Thread scheduling policy and priority. */
int
Thread::setScheduler ( int policy, int prio )
{
    struct sched_param param;
    int    err = 0;

    param.sched_priority = prio;

    if ( this->isRunning() )
    {
        if ( (err = ::pthread_setschedparam(_tid, policy, &param)) != 0 ) {
            _serr = "Error in pthread_setschedparam";
            if ( err == EINVAL )
                _serr.append(": Invalid policy or prio won't apply to this policy.");
            else if ( err == EPERM )
                _serr.append(": Invalid permissions to set policy/prio.");
        }
    }
    else
    {
        err = this->setSchedulerAttr(policy);
        if ( ! err )
            err = this->setPriorityAttr(prio);
    }


    return err;
}

/** Sets the scheduling policy of this thread. Standard pthread
 *  policies are SCHED_FIFO, SCHED_RR, or SCHED_OTHER.
 *  Note that SCHED_OTHER may not make use of priority
 *  since the true scheduling policy is intentionally undefined.
 */
int
Thread::setSchedulerAttr ( int policy )
{
    int err = 0;

    if ( this->isRunning() ) {
        _serr = "Thread already running";
        return 1;
    }

    if ( (err = ::pthread_attr_setschedpolicy(&_attr, policy)) != 0 ) {
        _serr = "Thread::setSchedulerAttr() error in pthread_attr_setschedpolicy.";
        if ( err == EINVAL || err == ENOTSUP )
            _serr.append(" Invalid value in policy.");
        return err;
    }

    if ( ! this->setInheritSched(PTHREAD_EXPLICIT_SCHED) ) {
        _serr = "Thread::setSchedulerAttr() error setting explicit thread scheduler.";
        return 1;
    }

    return err;
}

/** Returns the int value of this thread scheduling policy. */
int
Thread::getScheduler ( int & policy, int & prio )
{
    int err  = 0;
    struct sched_param param;

    if ( this->isRunning() ) {
        err = ::pthread_getschedparam(_tid, &policy, &param);
        if ( err == 0 )
            prio = param.sched_priority;
    } else {
        err  = ::pthread_attr_getschedpolicy(&_attr, &policy);
        if ( err )
            return err;
        err = this->getPriorityAttr(prio);
    }

    return err;
}

/* -------------------------------------------------------------- */

/** Sets the thread priority to the provided integer.
 *  This implies that we may be using realtime scheduling, which
 *  can be dangerous in terms of starving other threads. */
int
Thread::setPriorityAttr ( int prio )
{
    struct sched_param param;

    if ( this->isRunning() ) {
        _serr = "Thread already running";
        return 1;
    }

    param.sched_priority = prio;

    return(::pthread_attr_setschedparam(&_attr, &param));
}

/**  Provides the current Thread priority by setting 
  *  @param prio to pthread priority attribute.
 **/
int
Thread::getPriorityAttr ( int & prio )
{
    int    err  = 0;
    struct sched_param  param;

    if ( (err = ::pthread_attr_getschedparam(&_attr, &param)) == 0 )
        prio = param.sched_priority;

    return err;
}

/* -------------------------------------------------------------- */

/**  Set the Thread's contention scope, PTHREAD_SCOPE_PROCESS or
 *   PTHREAD_SCOPE_SYSTEM.
 */
bool
Thread::setScope ( int scope )
{
    if ( ::pthread_attr_setscope(&_attr, scope) )
        return false;

    return true;
}

/**  Returns the value of the Thread's current contention scope. */
int
Thread::getScope()
{
    int scope = 0;

    if ( ::pthread_attr_getscope(&_attr, &scope) )
        return 0;

    return scope;
}
/* -------------------------------------------------------------- */

bool
Thread::setInheritSched ( int inherit )
{
    if ( ::pthread_attr_setinheritsched(&_attr, inherit) )
        return false;
    return true;
}

int
Thread::getInheritSched()
{
    int inherit = 0;

    if ( ::pthread_attr_getinheritsched(&_attr, &inherit) )
        return 0;

    return inherit;
}

/* -------------------------------------------------------------- */

/** Abstract finished() implementation that sets the Thread's
 *  'running' boolean as indicated in a call to Thread::isRunning().
 *  Derived objects that wish to extend the finished function should
 *  consider this, or ensure that this base method is still called.
 */
void
Thread::finished()
{
    this->_running = false;
}

/**  Returns the last known error's string value, set by many methods
 *   that simply indicate boolean or integer result status.
 */
const std::string&
Thread::getErrorStr() const
{
    return _serr;
}

/* -------------------------------------------------------------- */

/**  The thread entry function used as the entry point for
 *   all new threads.
 **/
void*
Thread::ThreadEntry ( void * arg )
{
    Thread * t = (Thread*) arg;

    if ( t == NULL )
        return NULL;

    t->run();
    t->finished();

    return((void*)t);
}

/* -------------------------------------------------------------- */

/**  Returns the thread ID of the calling thread */
pthread_t
Thread::ThreadID()
{
    return(::pthread_self());
}

/* -------------------------------------------------------------- */

/**  Return the number of available system cpu's determined by
 *   sysconf(_SC_NPROCESSORS_CONF)
 */
long
Thread::MaxCPUs()
{
    return(::sysconf(_SC_NPROCESSORS_CONF));
}

/**  Returns the string name of the provided Thread's configured scheduler */
std::string
Thread::GetSchedulerPolicyName ( Thread * t )
{
    int  policy, prio;

    t->getScheduler(policy, prio);

    return( (policy == SCHED_FIFO)  ? std::string("SCHED_FIFO") :
            (policy == SCHED_RR)    ? std::string("SCHED_RR") :
            (policy == SCHED_OTHER) ? std::string("SCHED_OTHER") :
            std::string("???") );
}

/* -------------------------------------------------------------- */

} // namespace

//  _TCANETPP_THREAD_CPP_

