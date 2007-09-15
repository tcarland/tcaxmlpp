/**  Queue - A synchronized fifo queue 
  *
  *    @Author    tca@soe.sony.com
  *    @Version   0.6
 **/
#ifndef _TCANETPP_QUEUE_HPP_
#define _TCANETPP_QUEUE_HPP_

extern "C" {
# include <errno.h>
}
#include <queue>


#include "ThreadLock.h"


namespace tcanetpp {


#define MAX_QUEUE_SIZE 65535


/**  A Synchronized Queue class. The Queue class provides a
  *  thread-safe, synchronized interface to the C++ STL queue class.
 **/
template<class T> class Queue {

  public:

    /**  Default constructor */
    Queue() 
        : _maxsize(MAX_QUEUE_SIZE) 
    {}

    Queue ( size_t maxsize )
        : _maxsize(maxsize) 
    {}


    virtual ~Queue() { this->Clear(); }


    /**  Pushes an object onto the queue */
    int  push ( T object )
    {
        if ( this->size() == _maxsize || _mutex.lock() < 0 )
            return 0;
	
        _queue.push(object);
        _mutex.unlock();

        return 1;
    }
 

    /**  Pops an object off of the queue. This used to be 
      *  implemented as "T pop();" instead of the pass by
      *  reference, which was MUCH preferred, however the
      *  pass by reference is the only way we can get the 
      *  Queue to support both objects and object *'s.
     **/ 
    int  pop ( T &object )
    {
	if ( this->size() == 0 || _mutex.lock() < 0 )
	    return 0;

	object = _queue.front();
	_queue.pop();
	_mutex.unlock();

	return 1;
    }

    
    /**  For use by the popper to wait for notification of 
      *  items on the queue. This is not performed in pop()
      *  as some users of Queue might intentionally not wish
      *  to block in this class. 
     **/
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


    /**  Returns the current size of the queue */
    size_t size()
    {
        size_t  sz = 0;

        if ( _mutex.lock() < 0 )
            return sz;

        sz = _queue.size();
        _mutex.unlock();

	return sz;
    }
 

    /**  Returns true if the Queue is empty. */
    bool isEmpty()
    {
        bool empty = true;

        if ( _mutex.lock() < 0 )
            return empty;

        empty = _queue.empty();
        _mutex.unlock();

        return empty;
    }


    /**  Clears all elements of the queue. Not a typical
      *  queue operation, and the STL queue does not 
      *  provide erase() or clear(). Note that this is
      *  not for use with T* as no delete occurs and will
      *  result in a memory leak if used.
     **/
    void Clear()
    {
        if ( _mutex.lock() < 0 )
            return;

	while ( !_queue.empty() )
	    _queue.pop();

        _mutex.unlock();
    }


  private:

    std::queue<T>    _queue;
    size_t           _maxsize;
    ThreadLock       _mutex;


};


}  // namespace

#endif  //  _TCANETPP_QUEUE_HPP_
