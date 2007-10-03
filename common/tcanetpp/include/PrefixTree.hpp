/**  PrefixTree
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.3
  *
  *  $Id: PrefixTree.hpp,v 1.9 2003/10/03 21:27:19 tca Exp $
 **/
#ifndef _TCANETPP_PREFIXTREE_HPP_
#define _TCANETPP_PREFIXTREE_HPP_

extern "C" {
# include <pthread.h>
}

#include "patricia.h"
#include "Prefix.hpp"


namespace tcanetpp {


/**    A C++ template that wraps the use of the patricia trie implementation
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

    PrefixTree()
    {
	_pt   = pt_init();
	_hl   = NULL;
	_lock = false;
    }

    PrefixTree ( bool implicit_lock )
    {
	_pt   = pt_init();
	_hl   = NULL;
	_lock = implicit_lock;
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

	T object = (T) pt_matchRock(_pt, p.getCidr());

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
	    pthread_mutex_lock(&_mutex);

	return sz;
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

	if ( _hl )
	    pt_free(_pt, _hl);
	else
	    pt_free(_pt, &ptClearHandler);
	
	if ( _lock )
	    pthread_mutex_unlock(&_mutex);
    }


    void setFreeHandler ( nodeHandler_t handler )
    {
	_hl = handler;
    }


  protected:

    static void ptClearHandler ( uint32_t addr, uint8_t mb, void * rock )
    {
	T  obj = (T) rock;
	if ( obj )
	    delete obj;
	return;
    }


  private:

    ptNode_t*                    _pt;
    nodeHandler_t                _hl;

    bool                         _lock;
    pthread_mutex_t              _mutex;

};

} // namespace


#endif  //  _TCANETPP_PREFIXTREE_HPP_
