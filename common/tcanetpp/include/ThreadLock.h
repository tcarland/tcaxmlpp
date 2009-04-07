/**  $Id: ThreadLock.h,v 1.8 2003/09/29 04:40:57 tca Exp $
  *
  *   Threadlock provides a wrapper to a pthread conditional mutex.
  * 
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
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

    /** Releases the lock */
    virtual int unlock();


    /**  Trys to obtain the lock. The lock will be acquired if it
      *  is available and the function will return 1. A return of 
      *  0 indicates the lock failed (EBUSY), and -1 is returned 
      *  in case of a pthread failure in pthread_mutex_trylock()
     **/
    virtual int tryLock();
    inline  int trylock() { return this->tryLock(); }


  private:

    pthread_mutex_t    _mutex;
    pthread_cond_t     _items;

};


}  // namespace 

#endif  // _TCANETPP_THREADLOCK_H_

