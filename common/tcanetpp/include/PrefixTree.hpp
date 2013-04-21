/**
  * @file PrefixTree.h
  *
  *   A STL wrapper to the patricia tree (patricia.h)
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
#ifndef _TCANETPP_PREFIXTREE_HPP_
#define _TCANETPP_PREFIXTREE_HPP_

#ifdef USE_PTHREADS
extern "C" {
# include <pthread.h>
}
#endif

#include "patricia.h"
#include "IpAddr.h"


namespace tcanetpp {


/**    A C++ template that wraps the use of the radix tree implementation
  *  from 'patricia.h'. 
  *  
  *    The underlying trie handles user data as void pointers so as a result,
  *  this template, although not type specific, must be instantiated as a 
  *  container of object pointers.
  *
  *  The following example demonstrates usage:
  *  
  *  {
  *    PrefixTree<Object*>  pt;
  *
  *    IpAddr   pfx(addr, mask);
  *    Object*  obj = new Object(pfx);
  *
  *    if ( pt.insert(pfx, obj) )
  *        printf("Insert succeeded\n");
  *  }
  *  
 **/
template<class T> 
class PrefixTree {
 
  public:

    PrefixTree ( bool implicit_lock = false )
        : _pt(NULL),
          _freeHandler(NULL),
          _lock(implicit_lock)
    {
        _pt   = pt_init();
        this->init();
    }

    virtual ~PrefixTree()
    {
        this->clear();
        ::free(_pt);
#       ifdef USE_PTHREADS
        if ( _lock )
            pthread_mutex_destroy(&_mutex);
#       endif
    }


    int  insert  ( const IpAddr & p, T obj )
    {
        int result = 0;
        cidr_t  c  = p.getCidr();

        this->lock();
        result = pt_insert(_pt, &c, (void*) obj);
        this->unlock();

        return result;
    }


    T    remove  ( const IpAddr & p )
    {
        cidr_t  c  = p.getCidr();

        this->lock();
        T  object = (T) pt_remove(_pt, &c);
        this->unlock();

        return object;
    }

   
    T    exactMatch ( const IpAddr & p )
    {
        cidr_t  c  = p.getCidr();

        this->lock();
        T   object = (T) pt_match(_pt, &c);
        this->unlock();
            
        return object;
    }


    T    longestMatch ( IpAddr & p )
    {
        cidr_t  c  = p.getCidr();

        this->lock();
        T   object = (T) pt_matchLongest(_pt, &c);
        this->unlock();

        return object;
    }


    int  size()
    {
        int sz = 0;
        
        this->lock();
        sz  = pt_size(_pt);
        this->unlock();

        return sz;
    }

    
    int  nodes()
    {
        int cnt = 0;
        
        this->lock();
        cnt  = pt_nodes(_pt);
        this->unlock();
        
        return cnt;
    }

    size_t memUsage()
    {
        size_t sz    = 0;
        int    nodes = 0;

        this->lock();
        nodes = pt_nodes(_pt);
        sz    = (nodes * sizeof(ptNode_t));
        this->unlock();

        return sz;
    }


    size_t totalMemUsage()
    {
        return(memUsage() + (size() * sizeof(T)));
    }


    void clear()
    {
        this->lock();

        if ( _freeHandler )
            pt_free(_pt, _freeHandler);
        else
            pt_free(_pt, &PTClearHandler);

        this->unlock();
    }


    void setFreeHandler ( nodeHandler_t handler )
    {
        _freeHandler = handler;
    }

    void lock()
    {
#       ifdef USE_PTHREADS
        if ( this->_lock )
            pthread_mutex_lock(&_mutex);
#       endif
    }

    void unlock()
    {
#       ifdef USE_PTHREADS
        if ( this->_lock )
            pthread_mutex_unlock(&_mutex);
#       endif
    }



  protected:

    void init()
    {
#       ifdef USE_PTHREADS
        if ( this->_lock )
            pthread_mutex_init(&_mutex, NULL);
#       endif
    }

    static void PTClearHandler ( uint64_t addrA, uint64_t addrB, uint16_t mb, void * rock )
    {
        T  obj = (T) rock;
        if ( obj )
            delete obj;
        return;
    }


  private:

    ptNode_t*                    _pt;
    nodeHandler_t                _freeHandler;

    bool                         _lock;
#   ifdef USE_PTHREADS
    pthread_mutex_t              _mutex;
#   endif

};

} // namespace


#endif  //  _TCANETPP_PREFIXTREE_HPP_

