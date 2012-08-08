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


#define DEFAULT_QUEUE_MAXSIZE 65535



template<class ValueType> class SynchronizedQueue {

  public:

    SynchronizedQueue ( size_t maxsize = DEFAULT_QUEUE_MAXSIZE )
        : _maxSize(maxsize) 
    {}


    virtual ~SynchronizedQueue() { this->clear(); }


    int  push ( ValueType object )
    {
        if ( this->size() == _maxSize || _mutex.lock() < 0 )
            return 0;
	
        _queue.push(object);
        _mutex.unlock();

        return 1;
    }
 

    int  pop ( ValueType & object )
    {
	if ( this->size() == 0 || _mutex.lock() < 0 )
            return 0;

	object = _queue.front();
	_queue.pop();
	_mutex.unlock();

	return 1;
    }

    
    int  wait()
    {
        if ( _mutex.lock() == 0 )
            return _mutex.wait();
        return -1;
    }

    
    int  waitFor ( time_t seconds )
    {
        if ( _mutex.lock() < 0 )
            return -1;

        return _mutex.waitFor(seconds);
    }


    int  lock()
    {
    	return _mutex.lock();
    }

    int  unlock()
    {
    	return _mutex.unlock();
    }

    int  notify()
    {
    	return _mutex.notify();
    }


    size_t size()
    {
        size_t  sz = 0;

        if ( _mutex.lock() < 0 )
            return sz;

        sz  = _queue.size();
        _mutex.unlock();

        return sz;
    }


    bool empty() { return this->isEmpty(); }


    bool isEmpty()
    {
        bool empty = true;

        if ( _mutex.lock() < 0 )
            return empty;

        empty = _queue.empty();
        _mutex.unlock();

        return empty;
    }


    void clear()
    {
        if ( _mutex.lock() < 0 )
            return;

        while ( !_queue.empty() )
            _queue.pop();

        _mutex.unlock();
    }


    size_t maxSize()
    {
        return _maxSize;
    }


    void maxSize ( size_t maxsz )
    {
        _maxSize = maxsz;
    }


  private:

    std::queue<ValueType>    _queue;
    size_t                   _maxSize;
    ThreadLock               _mutex;

};


}  // namespace

#endif  //  _TCANETPP_QUEUE_HPP_

