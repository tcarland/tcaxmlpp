/**  Abstract ThreadLock class
  *
  *  @Author  tcarland@gmail.com
  *  @Version 1.3
 **/
#define _TCANETPP_THREADLOCK_CPP_

extern "C" {
# include <string.h>
# include <stdio.h>
# include <errno.h>
}

#include "ThreadLock.h"


namespace tcanetpp {


static const
char rcsid[] = "$Id: ThreadLock.cpp,v 1.7 2007/09/16 04:40:57 tca Exp $";


/* -=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


ThreadLock::ThreadLock()
{
    ::pthread_mutex_init(&_mutex, NULL);
    ::pthread_cond_init(&_items, NULL);
}


ThreadLock::~ThreadLock()
{
    ::pthread_mutex_destroy(&_mutex);
    ::pthread_cond_destroy(&_items);
}


int
ThreadLock::lock()
{
    if ( ::pthread_mutex_lock(&_mutex) != 0 )
	return -1;
    return 1;
}


int
ThreadLock::tryLock()
{
    int status;

    status = ::pthread_mutex_trylock(&_mutex);

    if ( status == EBUSY )
	return 0;
    else if ( status != 0 )
	return -1;
    return 1;
}


int
ThreadLock::unlock()
{
    ::pthread_mutex_unlock(&_mutex);
    return 1;
}


int
ThreadLock::wait()
{
    if ( ::pthread_cond_wait(&_items, &_mutex) != 0 )
	return -1;
    return 1;
}


int
ThreadLock::waitFor ( time_t seconds)
{
    struct timespec  to;
    int    status;

    if ( seconds < 1 )
	return 0;

    ::memset(&to, 0, sizeof(to));

    to.tv_sec = ::time(NULL) + seconds;
    status    = ::pthread_cond_timedwait(&_items, &_mutex, &to);

    if ( status == ETIMEDOUT )
	return 0;
    else if ( status != 0 )
	return -1;

    return 1;
}


int
ThreadLock::notify()
{
    return(::pthread_cond_signal(&_items));
}


int
ThreadLock::notifyAll()
{
    return(::pthread_cond_broadcast(&_items));
}


} // namespace


//  _TCANETPP_THREADLOCK_CPP
