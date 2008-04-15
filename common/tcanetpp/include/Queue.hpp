/**   
  *   A SynchronizedQueue implementation.
  *
  *    @Author    tcarland@gmail.com
  *    @Version   1.1
 **/
#ifndef _TCANETPP_QUEUE_HPP_
#define _TCANETPP_QUEUE_HPP_

extern "C" {
# include <errno.h>
}
#include <queue>


#include "ThreadLock.h"


namespace tcanetpp {


#define DEFAULT_MAXSIZE 65535


template<class ValueType> class SynchronizedQueue {

  public:

    SynchronizedQueue() 
        : _maxSize(DFAULT_MAXSIZE) 
    {}

    SynchronizedQueue ( size_t maxsize )
        : _maxSize(maxsize) 
    {}


    virtual ~SynchronizedQueue()
    { 
        this->clear(); 
    }


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
            return ThreadLock::wait();
        return -1;
    }

    
    int  waitFor ( time_t seconds )
    {
        if ( _mutex.lock() < 0 )
            return -1;

        return ThreadLock::waitFor(seconds);
    }


    size_t size()
    {
        size_t  sz = 0;

        if ( _mutex.lock() < 0 )
            return sz;

        sz = _queue.size();
        _mutex.unlock();

	return sz;
    }
 

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


  private:

    std::queue<ValueType>    _queue;
    size_t                   _maxSize;
    tcanetpp::ThreadLock     _mutex;


};


}  // namespace

#endif  //  _TCANETPP_QUEUE_HPP_
