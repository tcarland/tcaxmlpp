/**   
  * @file SynchronizedQueue.hpp
  *
  *   A thread-safe wrapper to the STL queue.
  *  
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author tcarland@gmail.com
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
#ifndef _TCANETPP_QUEUE_HPP_
#define _TCANETPP_QUEUE_HPP_

extern "C" {
# include <errno.h>
}
#include <queue>


#include "ThreadLock.h"


namespace tcanetpp {


#define DEFAULT_QUEUE_MAXSIZE 2^16


/**  A SynchronizedQueue is a thread-safe wrapper to a
  *  STL queue container. If the queue is used with pointers,
  *  care must be taken to properly delete the resources prior
  *  to destruction by popping all pointers off the queue first.
  *  Alternatively, the class can be derived and the clear() method 
  *  can be implemented to do this.
 **/
template<class ValueType> class SynchronizedQueue {

  public:

    SynchronizedQueue ( size_t maxsize = DEFAULT_QUEUE_MAXSIZE )
        : _maxSize(maxsize) 
    {}

    virtual ~SynchronizedQueue() 
    { 
        this->clear(); 
    }

    /**  Pushes an object onto the queue and returns a positive value
      *  on success or zero if the queue is full.
     **/
    int  push ( ValueType object )
    {
        if ( this->size() == _maxSize || this->lock() < 0 )
            return 0;

        _queue.push(object);
        this->unlock();

        return 1;
    }

    /**  Pops an object off the front of the queue returning a positive
      *  value on success or zero if the queue is empty.
     **/
    int  pop ( ValueType & object )
    {
        if ( this->size() == 0 || this->lock() < 0 )
            return 0;

        object = _queue.front();
        _queue.pop();
        this->unlock();

        return 1;
    }

    /**  This locks and waits the calling thread on the underlying mutex 
      *  until notified via the notify() method. 
     **/
    int  wait()
    {
        if ( _mutex.lock() == 0 )
            return _mutex.wait();
        return -1;
    }

    /**  Will lock the calling thread for the time specified.
      *  @param  usec  is the number of microseconds to wait.
     **/
    int  waitFor ( time_t usec )
    {
        if ( _mutex.lock() < 0 )
            return -1;
        return _mutex.waitFor(usec);
    }

    /**  Will lock the calling thread for the time specified.
      *  @param  ts  is a timespec struct specifying the 
      *  length of time to wait.
     **/
    int  waitFor ( const timespec * ts )
    {
        if ( _mutex.lock() < 0 )
            return -1;
        return _mutex.waitFor(ts);
    }

    /**  Notifies any waiting threads */
    inline int  notify()
    {
        return _mutex.notify();
    }

    /**  Returns the current size of the queue */
    size_t size()
    {
        size_t  sz = 0;

        if ( this->lock() < 0 )
            return sz;

        sz  = _queue.size();
        this->unlock();

        return sz;
    }

    /**@{
      *  Returns a boolean indicating whether the queue is empty. 
     **/
    inline bool empty() 
    { 
        return this->isEmpty(); 
    }

    bool isEmpty()
    {
        bool empty = true;

        if ( this->lock() < 0 )
            return empty;

        empty = _queue.empty();
        this->unlock();

        return empty;
    }
    /*@}*/

    /**  Clears or empties the queue of all objects. If this is a 
      *  container of object pointers, no resources would be free'd
      *  resulting in a leak, in which case the queue should be 
      *  cleared manually.
     **/
    virtual void clear()
    {
        if ( this->lock() < 0 )
            return;

        while ( !_queue.empty() )
            _queue.pop();

         this->unlock();
    }

    /**  Returns the maximum queue size allowed. */
    inline size_t maxSize()
    {
        return _maxSize;
    }

    /**  @param maxsz sets the maximum queue size accordingly */
    inline void maxSize ( size_t maxsz )
    {
        this->lock();
        _maxSize = maxsz;
        this->unlock();
    }

    inline int  lock()
    {
        return _mutex.lock();
    }

    inline int  unlock()
    {
        return _mutex.unlock();
    }

  private:

    std::queue<ValueType>    _queue;
    size_t                   _maxSize;
    ThreadLock               _mutex;

};


}  // namespace

#endif  //  _TCANETPP_QUEUE_HPP_

