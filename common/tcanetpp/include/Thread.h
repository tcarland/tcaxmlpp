/**  
  * @file Thread.h
  *
  *    An abstract Thread class wrapping libpthread.
  *  
  * Copyright (c) 2002,2008 Timothy Charlton Arland 
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
#ifndef _TCANETPP_THREAD_H_
#define _TCANETPP_THREAD_H_

extern "C" {
# include <pthread.h>
# include <sched.h>
# include <time.h>
}
#include <string>

#include "Exception.hpp"


namespace tcanetpp {


#define THREAD_STACKSIZE_MIN 16384


/** 
  *  ThreadException class.  
 **/
class ThreadException : public Exception {
  public:
    ThreadException ( const std::string & s_err ) 
        : Exception(s_err) {}
};


/**
  *  The Thread class is an abstract interface class for instantiating an object
  *  as a thread. The derived object provides the 'run()' implementation.
  *  The thread is started via the base class's non-virtual method 'start()', 
  *  which sets up the thread and makes the call to the virtual 'init() method 
  *  for any needed initialization, and then the derived 'run()'.
  *  On thread exit or termination, the derived virtual 'finished()'.
  *  Finally, stop() will terminate the thread and 'join' if applicable.
  *  setAlarm() allows for the signaling of the underlying thread 
  *  to initiate a graceful 'stop'.
 **/
class Thread {

  public:

    Thread ( bool detach = false );
    virtual ~Thread();


    virtual void        run ( void ) = 0;

    virtual void        init() {}
    virtual void        finished();
    virtual void        setAlarm();


    void                start() throw ( ThreadException );
    void                stop()  throw ( ThreadException );


    bool                isRunning();
    void                yield();

    void                threadName      ( const std::string & name );
    const std::string&  threadName() const;

    const std::string&  getErrorStr() const;

    bool                setStackSize    ( size_t stksz );
    bool                getStackSize    ( size_t & stksz );

    bool                setCpuAffinity  ( long cpu );
    bool                getCpuAffinity  ( cpu_set_t & cpus );

    /* scheduling parameters */
    int                 getMinPriority  ( int policy );
    int                 getMaxPriority  ( int policy );

    int                 setScheduler    ( int policy, int prio );

    int                 setSchedulerAttr( int policy );
    int                 getScheduler    ( int & policy, int & prio );

    int                 setPriorityAttr ( int prio );
    int                 getPriorityAttr ( int & prio );

    bool                setScope        ( int scope );
    int                 getScope();

    bool                setInheritSched ( int inherit = PTHREAD_INHERIT_SCHED );
    int                 getInheritSched();


    static long         MaxCPUs();
    static std::string  GetSchedulerPolicyName ( Thread * t );

    static pthread_t    ThreadID();

  private:

    static void*        ThreadEntry ( void * arg );


  protected:

    std::string         _threadName;
    std::string         _serr;
    bool                _Alarm;
    bool                _running;
    bool                _detach;


  private:

    pthread_t           _tid;
    pthread_attr_t      _attr;
    struct sched_param  _param;
    void*               _stack;

};


}  // namespace


#endif  // _TCANETPP_THREAD_H_


/* -------------------------------------------------------------- */

/**  Simple Thread class example:
  *  
  *  class CacheMgrThread : public Thread {
  *    public:
  *      string msgStr;
  *      CacheMgrThread ( string msg ) : msgStr(msg) {}
  *      virtual ~CacheMgrThread() {}
  *      virtual void run() {
  *        printf("%s started!!\n", this->threadName().c_str());
  *        while ( !_Alarm ) {
  *          printf("%s\n", msgStr.c_str());
  *          sleep(5);
  *        }
  *        printf("%s received alarm!\n", this->threadName().c_str());
  *      }
  *  };
  *
  *  int main() {
  *    string msg1 = "blah blah black sheep!";
  *    string msg2 = "blah blah white sheep!";
  *    printf("starting in main\n");
  *    CacheMgrThread *cc1 = new CacheMgrThread();
  *    CacheMgrThread *cc2 = new CacheMgrThread();
  *    cc1->start();
  *    cc2->start();
  *    sleep(60);
  *    printf("stopping\n");
  *    cc1->stop();
  *    cc2->stop();
  *    printf("finished\n");
  *    delete(cc1);
  *    delete(cc2);
  *    return 0;
  *  }
 **/

