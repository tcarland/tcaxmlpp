/**
  *  ThreadMutexPool.cpp
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
#define _TCANETPP_THREADMUTEXPOOL_CPP_

#include <algorithm>

#include "ThreadMutexPool.h"


namespace tcanetpp {


ThreadMutexPool::ThreadMutexPool()
    : _lockcnt(0),
      _lockmin(DEFAULT_MUTEXPOOL_MINSIZE),
      _lockmax(DEFAULT_MUTEXPOOL_MAXSIZE)
{
   this->createMutexes();
}


ThreadMutexPool::~ThreadMutexPool()
{
    ThreadLockPool::iterator  tIter;

    this->lock();
    
    for ( tIter = _mutexIn.begin(); tIter != _mutexIn.end(); ++tIter ) {
        ThreadLock * lock = (ThreadLock*) *tIter;
        
        if ( lock )
            delete lock;
    }
    _mutexIn.clear();
 
    for ( tIter = _mutexOut.begin(); tIter != _mutexOut.end(); ++tIter ) {
        ThreadLock * lock = (ThreadLock*) *tIter;
        
        if ( lock )
            delete lock;
    }
    _mutexOut.clear();
}


ThreadLock*
ThreadMutexPool::AcquireMutex()
{
    ThreadLock * lock = NULL;

    this->lock();

    if ( _mutexIn.empty() )
        this->createMutexes();

    if ( ! _mutexIn.empty() ) {
        lock = _mutexIn.front();
        _mutexIn.pop_front();
        _mutexOut.push_front(lock);
    }

    this->unlock();

    return lock;
}


void
ThreadMutexPool::ReleaseMutex ( ThreadLock * lock )
{
    ThreadLockPool::iterator  tIter;

    this->lock();

    tIter = find(_mutexOut.begin(), _mutexOut.end(), lock);

    if ( tIter == _mutexOut.end() ) {
        // wtf..delete it anyway! that'll teach em
        if ( lock )
            delete lock;
    } else {
       _mutexOut.erase(tIter);
        _mutexIn.push_front(lock);
    }

    this->unlock();

    return;
}


void
ThreadMutexPool::MinPoolSize ( size_t minsize )
{
    _lockmin = minsize;
}


size_t
ThreadMutexPool::MinPoolSize()
{
    return _lockmin;
}


void
ThreadMutexPool::MaxPoolSize ( size_t maxsize )
{
    _lockmax = maxsize;
}

size_t
ThreadMutexPool::MaxPoolSize()
{
    return _lockmax;
}


size_t
ThreadMutexPool::MutexAvailable()
{
    return(_lockmax - _mutexOut.size());
}

size_t
ThreadMutexPool::MutexInUse()
{
    return _mutexOut.size();
}



void
ThreadMutexPool::lock()
{
    _mutex.lock();
}

void
ThreadMutexPool::unlock()
{
    _mutex.unlock();
}

void
ThreadMutexPool::createMutexes()
{
    ThreadLock * lock = NULL;
    
    if ( _lockcnt > _lockmax )
        return;

    int num = (int) ((_lockmax - _lockmin) * .05) / 2;

    if ( num > _lockmin )
        num = _lockmin;
    else if ( num < 1 )
        num = 1;

    if ( (_lockcnt + num) > _lockmax )
        num = _lockmax - _lockcnt;

    for ( int i = 0; i < num; i++ ) {
        lock = new ThreadLock();
        _mutexIn.push_front(lock);
    }

    return;
}


} // namespace


//  _TCANETPP_THREADMUTEXPOOL_CPP_

