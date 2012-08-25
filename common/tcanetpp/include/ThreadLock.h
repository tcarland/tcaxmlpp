/**
  * @file ThreadLock.h
  *
  *   Threadlock wraps a pthread conditional mutex.
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
#ifndef _TCANETPP_THREADLOCK_H_
#define _TCANETPP_THREADLOCK_H_

extern "C" {
# include <pthread.h>
# include <time.h>
}


namespace tcanetpp {


class ThreadLock;


/**  Provides a convenient method for synchronizing a function by
  *  creating an instance of this class on the stack with the
  *  given mutex to lock which will automatically unlock the
  *  mutex at the end of function life (stack destruction)
 **/
class ThreadAutoMutex {

  public:

    ThreadAutoMutex ( ThreadLock * lock, bool sync = true );
    virtual ~ThreadAutoMutex();

  private:

    ThreadLock *  _mutex;
    bool          _sync;
};


/**  The ThreadLock class provides a convenient wrapper to
  *  a pthread mutex.
 */
class ThreadLock {

  public:

    ThreadLock();
    virtual ~ThreadLock();


    virtual int wait();
    virtual int waitFor ( time_t seconds );

    virtual int lock();
    virtual int unlock();

    virtual int notify();
    virtual int notifyAll();

    virtual int tryLock();
    inline  int trylock() { return this->tryLock(); }

  private:

    pthread_mutex_t    _mutex;
    pthread_cond_t     _items;

};


}  // namespace 

#endif  // _TCANETPP_THREADLOCK_H_

