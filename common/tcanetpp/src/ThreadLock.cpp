/**
  *   ThreadLock class
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
#define _TCANETPP_THREADLOCK_CPP_

extern "C" {
# include <string.h>
# include <stdio.h>
# include <errno.h>
}

#include "ThreadLock.h"


namespace tcanetpp {


static const
char rcsid[] = "$Id: ThreadLock.cpp,v 1.7 2008/08/18 04:40:57 tca Exp $";


/* ----------------------------------------------------------------------- */


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


//  _TCANETPP_THREADLOCK_CPP_

