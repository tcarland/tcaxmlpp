#ifndef _TCANETPP_THREADMUTEXPOOL_H_
#define _TCANETPP_THREADMUTEXPOOL_H_

#include <list>

#include "ThreadLock.h"


namespace tcanetpp {


#define DEFAULT_MUTEXPOOL_MINSIZE 5
#define DEFAULT_MUTEXPOOL_MAXSIZE 100


typedef std::list<ThreadLock*>  ThreadLockPool;


/**  A class for managing a resource pool of mutexes. */
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

    void         lock();
    void         unlock();
    void         createMutex();

    
  protected:

    ThreadLockPool    _mutexIn;
    ThreadLockPool    _mutexOut;

    ThreadLock        _mutex;

    int               _lockcnt;
    int               _lockmin;
    int               _lockmax;

};


} // namespace

#endif // _TCANETPP_THREADMUTEXPOOL_H_
