/**
  *  ThreadAutoMutex.hpp
  *
  * Copyright (c) 2008,2009 Timothy Charlton Arland 
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
#ifndef _TCANETPP_THREADAUTOMUTEX_HPP_
#define _TCANETPP_THREADAUTOMUTEX_HPP_

#include "ThreadLock.h"

namespace tcanetpp {


/**  Intended to simplify the synchronization of a class method.  */
class ThreadAutoMutex {

  public:

    explicit ThreadAutoMutex ( ThreadLock * lock )
        : _mutex(lock)
    {
        this->_mutex->lock();
    }

    ~ThreadAutoMutex()
    {
        this->_mutex->unlock();
    }


  private:

    ThreadLock*  _mutex;
};


}  // namespace

#endif  // _TCANETPP_THREADAUTOMUTEX_HPP_

