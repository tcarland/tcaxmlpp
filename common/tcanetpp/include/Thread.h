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


/** -=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- **/
/** 
  *  ThreadException class.  
 **/
class ThreadException : public Exception {
  public:
    ThreadException ( const std::string & s_err ) 
        : Exception(s_err) {}
};


/** -=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- **/
/**
  *  The Thread class is an abstract class for instantiating an object
  *  as a thread. A derived object must provide its own "void run()" 
  *  method (as in java's Runnable interface). The thread is started
  *  via the base class' non-virtual methods, start() and stop(),
  *  which uses a simple _Alarm boolean to indicate that the thread should
  *  terminate operation.
 **/
class Thread {


  public:

    Thread();
    Thread ( bool detach );
    virtual ~Thread();


    /**  The run method is to be implemented by the extending class
      *  and is called by the thread entry point at thread start.
     **/
    virtual void run ( void ) = 0;


    /**  This is a convenience method called prior to run() by the
      *  thread entry function
     **/
    virtual void init() {}


    /**  Performs the function of starting the thread.
      *  The method will throw a ThreadException if the call to
      *   pthread_create fails, or the start method has already
      *  been called.
     **/
    void start() throw ( ThreadException );

    /**  Performs the function of forcibly stopping the thread.
      *  The method will throw a ThreadException if the call
      *  to pthread_join fails.
     **/
    void stop() throw ( ThreadException );


    /**  Function called by the threadEntry when run() is finished.  */
    virtual void finished();


    /**  Sets a cosmetic name for this thread */
    void threadName ( const std::string & name );


    /**  Returns the id name of this thread */
    const std::string& threadName() const;


    /**  Returns a boolean indicating whether the thread is 
      *  currently running.
     **/
    bool isRunning();


    /**  Method to set the alarm boolean to true */
    virtual void setAlarm();


  protected:

    /**  This is protected because it should be used internally by
      *  this thread to call sched_yield() in the current thread
      *  context only.
     **/
    void yield();


  private:

    /**  The thread entry point */
    static void * threadEntry ( void * arg );


  protected:

    std::string     _threadName;
    std::string     _serr;
    bool            _Alarm;
    bool            _running;
    bool            _detach;


  private:

    pthread_t       _tid;

};


}  // namespace


#endif  // _TCANETPP_THREAD_H_


/** -=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- **/

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

