/**  $Id: PrefixTree.hpp,v 1.9 2003/10/03 21:27:19 tca Exp $
  *
  *   PrefixTree - stl wrapper to a patricia tree
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
#ifndef _TCANETPP_PREFIXTREE_HPP_
#define _TCANETPP_PREFIXTREE_HPP_

extern "C" {
# include <pthread.h>
}

#include "patricia.h"
#include "Prefix.hpp"


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
  *    Prefix   pfx(addr, mask);
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
        : _freeHandler(NULL),
          _lock(implicit_lock)
    {
        _pt   = pt_init();
        if ( _lock )
            pthread_mutex_init(&_mutex, NULL);
    }

    virtual ~PrefixTree()
    {
        this->clear();
        free(_pt);
        if ( _lock )
            pthread_mutex_destroy(&_mutex);
    }


    int  insert  ( Prefix & p, T obj )
    {
        int result = 0;

        if ( _lock )
            pthread_mutex_lock(&_mutex);
            
        result = pt_insert(_pt, p.getCidr(), (void*) obj);

        if ( _lock )
            pthread_mutex_unlock(&_mutex);

        return result;
    }


    T    remove  ( Prefix & p )
    {
        if ( _lock )
            pthread_mutex_lock(&_mutex);

        T object = (T) pt_remove(_pt, p.getCidr());

        if ( _lock )
            pthread_mutex_unlock(&_mutex);

        return object;
    }

   
    T    exactMatch ( Prefix & p )
    {
        if ( _lock )
            pthread_mutex_lock(&_mutex);

        T object = (T) pt_match(_pt, p.getCidr());

        if ( _lock )
            pthread_mutex_unlock(&_mutex);
            
        return object;
    }


    T    longestMatch ( Prefix & p )
    {
        if ( _lock )
            pthread_mutex_lock(&_mutex);

        T object = (T) pt_matchLongest(_pt, p.getCidr());

        if ( _lock )
            pthread_mutex_unlock(&_mutex);

        return object;
    }


    int  size()
    {
        if ( _lock )
            pthread_mutex_lock(&_mutex);

        int sz = pt_size(_pt);

        if ( _lock )
            pthread_mutex_unlock(&_mutex);

        return sz;
    }

    
    int  nodes()
    {
        if ( _lock )
            pthread_mutex_lock(&_mutex);
        
        int cnt = pt_nodes(_pt);
        
        if ( _lock )
            pthread_mutex_unlock(&_mutex);
        
        return cnt;
    }

    size_t memUsage()
    {
        size_t sz    = 0;
        int    nodes = 0;

        if ( _lock )
            pthread_mutex_lock(&_mutex);

        nodes = pt_nodes(_pt);

        if ( _lock )
            pthread_mutex_unlock(&_mutex);

        sz = (nodes * sizeof(ptNode_t));

        return sz;
    }


    size_t totalMemUsage()
    {
        return(memUsage() + (size() * sizeof(T)));
    }


    void clear()
    {
        if ( _lock )
            pthread_mutex_lock(&_mutex);

        if ( _freeHandler )
            pt_free(_pt, _freeHandler);
        else
            pt_free(_pt, &PTClearHandler);

        if ( _lock )
            pthread_mutex_unlock(&_mutex);
    }


    void setFreeHandler ( nodeHandler_t handler )
    {
        _freeHandler = handler;
    }


  protected:

    static void PTClearHandler ( uint32_t addr, uint8_t mb, void * rock )
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
    pthread_mutex_t              _mutex;

};

} // namespace


#endif  //  _TCANETPP_PREFIXTREE_HPP_

