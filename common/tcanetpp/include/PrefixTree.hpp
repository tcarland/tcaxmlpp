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

    /**  Default Constructor, which does not enable mutex locking. */
    PrefixTree()
    {
	_pt   = pt_init();
	_hl   = NULL;
	_lock = false;
    }

    /**  Constructor
      *  @param  implicit_lock  boolean indicating whether to enable
      *     mutex locking.
     **/
    PrefixTree ( bool implicit_lock )
    {
	_pt   = pt_init();
	_hl   = NULL;
	_lock = implicit_lock;
	if ( _lock )
	    pthread_mutex_init(&_mutex, NULL);
    }

    /**  Default destructor */
    ~PrefixTree()
    {
	this->clear();
	free(_pt);
	if ( _lock )
	    pthread_mutex_destroy(&_mutex);
    }


    /**  The insert() method inserts a prefix and associated object into
      *  the internal patricia trie and returns 1 on success or 0 on failure.
      *
      *  @param  p    is a Prefix object representing x.x.x.x/yy
      *  @param  obj  is the object pointer associated with the prefix.
     **/
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


    /**  Removes the given prefix from the patricia returning the associated 
      *  object, if any. Returns NULL if the given prefix is not found.
      *
      *  @param  p  is the Prefix object representing the prefix to remove
      *     from the underlying trie.
     **/
    T    remove  ( Prefix & p )
    {
	if ( _lock )
	    pthread_mutex_lock(&_mutex);

	T object = (T) pt_remove(_pt, p.getCidr());

	if ( _lock )
	    pthread_mutex_unlock(&_mutex);

	return object;
    }

   
    /**  Performs an exact match lookup for the given prefix and returns the
      *  associated object or NULL if not found.
      *
      *  @param  p  is the Prefix object representing the prefix to match.
     **/
    T    exactMatch ( Prefix & p )
    {
	if ( _lock )
	    pthread_mutex_lock(&_mutex);

	T object = (T) pt_matchRock(_pt, p.getCidr());

	if ( _lock )
	    pthread_mutex_unlock(&_mutex);
	    
	return object;
    }


    /**  Performs a longest match lookup for the given prefix and returns
      *  the object of the longest matching prefix found or NULL if no
      *  valid prefix exists.
      *
      *  @param  p  is the Prefix object representing the prefix to match.
     **/
    T    longestMatch ( Prefix & p )
    {
	if ( _lock )
	    pthread_mutex_lock(&_mutex);

	T object = (T) pt_matchLongest(_pt, p.getCidr());

	if ( _lock )
	    pthread_mutex_unlock(&_mutex);

	return object;
    }


    /**  Returns the current size of the patricia by counting the number
      *  of allocated children (note that this does not usually equal the
      *  number of allocated nodes).
     **/
    int  size()
    {
	if ( _lock )
	    pthread_mutex_lock(&_mutex);

        int sz = pt_size(_pt);

	if ( _lock )
	    pthread_mutex_lock(&_mutex);

	return sz;
    }


    /**  Returns the memory utilization of the patricia in bytes. Note that
      *  this does NOT include memory allocated to the children objects (T)
      *  stored in the trie.
     **/
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


    /**  Returns the total memory allocated to both the internal trie
      *  and user allocated (T) objects.
     **/
    size_t totalMemUsage()
    {
	return(memUsage() + (size() * sizeof(T)));
    }


    /**  Clears all nodes of the trie using either the internal handler
      *  or the user supplied handler from setFreeHandler().
     **/
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


    /**  Allows a user supplied callback to be used when clearing the trie.
      *  This is useful if post-processing is needed on the stored (T) 
      *  objects prior to deletion.
     **/
    void setFreeHandler ( nodeHandler_t handler )
    {
	_hl = handler;
    }


  protected:

    /**  Internal handler used as the default free handler for clearing 
      *  the trie.
     **/
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
