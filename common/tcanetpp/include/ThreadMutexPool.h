/**
  * @file ThreadMutexPool.h
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author tca@charltontechnology.net
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
#ifndef _TCANETPP_THREADMUTEXPOOL_H_
#define _TCANETPP_THREADMUTEXPOOL_H_

#include <list>

#include "ThreadLock.h"


namespace tcanetpp {


#define DEFAULT_MUTEXPOOL_MINSIZE 5
#define DEFAULT_MUTEXPOOL_MAXSIZE 100


typedef std::list< ThreadLock* >  ThreadLockPool;


/**  A class for managing a resource pool of pthread mutex's 
  *  represented by a tcanetpp::ThreadLock instance. 
 **/
class ThreadMutexPool {

  public:

    ThreadMutexPool();
    virtual ~ThreadMutexPool();


    ThreadLock*  AcquireMutex();
    void         ReleaseMutex ( ThreadLock * lock );

    void         MinPoolSize ( size_t size );
    size_t       MinPoolSize();
    
    void         MaxPoolSize ( size_t size );
    size_t       MaxPoolSize();

    size_t       MutexAvailable();
    size_t       MutexInUse();


  private:

    void         createMutexes();

    
  protected:

    ThreadLockPool    _mutexIn;
    ThreadLockPool    _mutexOut;

    ThreadLock        _lock;

    int               _lockcnt;
    int               _lockmin;
    int               _lockmax;

};


} // namespace

#endif // _TCANETPP_THREADMUTEXPOOL_H_

