/**  Threadlock base class
  *
  *  @Author  tcarland@gmail.com
  *  @Version 1.3
  *
  *  $Id: ThreadLock.h,v 1.8 2003/09/29 04:40:57 tca Exp $
 **/
#ifndef _TCANETPP_THREADLOCK_H_
#define _TCANETPP_THREADLOCK_H_

extern "C" {
# include <pthread.h>
# include <time.h>
}


namespace tcanetpp {


/** -=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- **/
/**  The ThreadLock class is a simple wrapper for a pthread mutex and
  *  condition variable intended primarily for providing wait/notify
  *  behavior to a derived Thread object. The methods lock(), tryLock(),
  *  and unlock() each exhibit the behaviors of the corresponding pthread
  *  calls to pthread_mutex_(lock|trylock|unlock). 
  *  This class was NOT intended to offer any additional safety or deadlock 
  *  protection from the misuse of a pthread mutex, but is perfectly safe
  *  if used correctly.
 **/ 
class ThreadLock {

  public:

    ThreadLock();
    virtual ~ThreadLock();

     
    /**  A blocking wait condition
      *
      *  Important: the mutex MUST be locked first. This function
      *  intentionally does NOT call lock() on the mutex.
     **/
    virtual int wait();
    

    /**  A timed wait that blocks for @param seconds.
      *
      *  Important: the mutex MUST be locked first. This function
      *  intentionally does NOT call lock() on the mutex.
     **/
    virtual int waitFor ( time_t seconds );


    /**  Signals a thread that may be waiting for a lock. */
    virtual int notify();
    

    /**  Signals all threads that may be waiting for a lock */
    virtual int notifyAll();
    
     
    /**  Obtains the lock. This is a blocking operation which 
      *  returns 1 on success.
     **/
    virtual int lock();

    /**  Trys to obtain the lock. The lock will be acquired if it
      *  is available and the function will return 1. A return of 
      *  0 indicates the lock failed (EBUSY), and -1 is returned 
      *  in case of a pthread failure in pthread_mutex_trylock()
     **/
    virtual int tryLock();
    inline  int trylock() { return this->tryLock(); }

    /** Releases the lock */
    virtual int unlock();


  private:

    pthread_mutex_t    _mutex;
    pthread_cond_t     _items;

};


}  // namespace 

#endif  // _TCANETPP_THREADLOCK_H_
