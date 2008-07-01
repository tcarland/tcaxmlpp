/**  Abstract Thread base class
  *
  *  @Author  tcarland@gmail.com
  *  @Version 1.8
  *
  *  $Id: Thread.h,v 1.8 2003/09/29 04:40:57 tca Exp $
 **/
#ifndef _TCANETPP_THREAD_H_
#define _TCANETPP_THREAD_H_

extern "C" {
# include <pthread.h>
# include <time.h>
}
#include <string>

#include "Exception.hpp"


namespace tcanetpp {


/** 
  *  ThreadException class.  
 **/
class ThreadException : public Exception {
  public:
    ThreadException ( const std::string & s_err ) 
        : Exception(s_err) {}
};


/**
  *  The Thread class is an abstract class for instantiating an object
  *  as a thread. A derived object must provide its own 'run()' implementation.
  *  The thread is started via the base class's non-virtual method 'start()', 
  *  which sets up the thread and makes the call to the virtual 'init() method 
  *  to provide custom thread initialization routines, and then 'run()'.
  *  On thread exit or termination, the users' virtual method 'finished()' is 
  *  called. Finally, stop() will terminate the thread and 'join' if appropriate. 
  *  The thread will forcibly call 'stop()' and potentially attempt to 'join' 
  *  the thread if the object is destroyed without the user stopping it.
  *  setAlarm() allows for the signaling of the underlying thread 
  *  implementation as a nice 'stop'.
 **/
class Thread {


  public:

    Thread();
    Thread ( bool detach );
    virtual ~Thread();


    virtual void        run ( void ) = 0;


    virtual void        init() {}

    void                start() throw ( ThreadException );
    void                stop()  throw ( ThreadException );


    virtual void        finished();

    void                threadName ( const std::string & name );
    const std::string&  threadName() const;


    bool                isRunning();
    virtual void        setAlarm();


  protected:

    void                yield();


  private:

    static void*        threadEntry ( void * arg );


  protected:

    std::string         _threadName;
    std::string         _serr;
    bool                _Alarm;
    bool                _running;
    bool                _detach;


  private:

    pthread_t           _tid;

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

