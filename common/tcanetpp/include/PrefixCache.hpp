/**
  * @file PrefixCache.hpp
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland
  * @author  tca@charltontechnology.net
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
#ifndef _TCANETPP_PREFIXCACHE_HPP_
#define _TCANETPP_PREFIXCACHE_HPP_

#include <set>
#include <list>

#include "PrefixTree.hpp"


namespace tcanetpp {


/**  Used as the Radix Node in the underlying PrefixTree of a
 *   PrefixCache instance. The PrefixCacheItem wraps the 
 *   given ValueType with an associated timer.
 **/
template <typename ValueType>
class PrefixCacheItem {

  public:

    typedef PrefixCacheItem<ValueType>      CacheItem;
    typedef std::pair< time_t, CacheItem* > CachePair;
    typedef std::multiset< CachePair >      TimerSet;
    typedef typename TimerSet::iterator     TimerSetIter;
    typedef TimerSetIter                    Timer;

  public:

    PrefixCacheItem ( const Prefix & p, ValueType item, int expire )
        : _prefix(p),
          _value(item),
          _expire(expire)
    {}

    virtual ~PrefixCacheItem() {}

    void           timeout ( long val )   { _expire = timeout; }
    long           timeout()              { return _expire; }

    const Prefix&  getPrefix()            { return _prefix; }
    Timer          getTimer()             { return _timer; }
    ValueType      getValue()             { return _value; }

    void           setTimer ( Timer & t ) { this->_timer = t; }


  private:

    Prefix         _prefix;
    ValueType      _value;
    Timer          _timer;
    long           _expire;
};


/**  The PrefixCache class provides an interface for tracking objects 
 *   by a given IPV4 Prefix. It uses an underlying Radix Tree to provide 
 *   a longest match lookup for the cache.
 **/
template <typename ValueType>
class PrefixCache {

  public:
    
    typedef PrefixCacheItem<ValueType>        CacheItem;
    typedef typename CacheItem::CachePair     CachePair;
    typedef typename CacheItem::TimerSet      CacheTimerSet;
    typedef typename CacheTimerSet::iterator  TimerSetIter;
    typedef PrefixTree<CacheItem*>            CacheTree;

  public:

    PrefixCache ( time_t refreshInterval, bool implicit_lock = false )
        : _pt(new CacheTree(implicit_lock)),
          _cacheTimeout(refreshInterval),
          _lock(implicit_lock)
    {}

    virtual ~PrefixCache() {}


    /**  Performs a lookup of the given 'Prefix', and return false if 
     *   the prefix already exists or insert fails and true on 
     *   successful insert.
     **/
    bool insert ( Prefix & p, ValueType item, const time_t & now )
    {
        CacheItem * ci = this->_pt->exactMatch(p);

        if ( ci != NULL )
            return false;

        TimerSetIter  timer;

        ci    = new CacheItem(p, item, now);
        timer = _timers.insert(CachePair(now + _cacheTimeout, ci));

        ci->setTimer(timer);

        if ( this->_pt->insert(p, ci) == 0 ) {
            delete ci;
            return false;
        }

        return true;
    }


    /** Removes the cache entry for the given Prefix and returns
     *  the corresponding template object.
     **/
    ValueType remove ( Prefix & p )
    {
        ValueType   val;
        CacheItem * ci = this->_pt->exactMatch(p);

        if ( ci == NULL || ci.getPrefix() != p )
            return val;

        val = ci->getValue();
        ci  = this->_pt->remove(p);

        if ( ci )
            delete ci;

        return val;
    }


    /**  Performs an exact match of the given Prefix and returns 
     *   return the success of the lookup and sets the provided 
     *   template object parameter accordingly.
     **/
    bool  match  ( const Prefix & p, ValueType & val )
    {
        CacheItem * ci = this->_pt->exactMatch(p);

        if ( ci == NULL )
            return false;

        val = ci->getValue();

        return true;
    }

    /**  Performs a longest match lookup for the given Prefix
     *   and retur the success of the lookup setting the 
     *   target parameter accordingly.
     **/
    bool longestMatch ( const Prefix & p, ValueType & val )
    {
        CacheItem * ci = this->_pt->longestMatch(p);

        if ( ci == NULL )
            return false;

        val = ci->getValue();

        return true;
    }

    /**  Updates the timer for the given Prefix to the provided 
     *   time plus the configured cache timeout interval.
     *   Returns false if the prefix is not found.
     **/
    bool refresh ( Prefix & p, time_t & now )
    {
        TimerSetIter  timer;
    	CacheItem    *ci  = this->_pt->exactMatch(p);

    	if ( NULL == ci || ci.getPrefix().getPrefix() == 0 )
            return false;

        timer = ci->getTimer();
    	ci->setExpireTime(now + _cacheTimeout);
    	_timers.erase(timer);
    	timer = _timers.insert(CachePair(ci->getExpireTime(), ci));
    	ci->setTimer(timer);

    	return true;
    }


    /**  Expires any entries that are older than the given @param now. 
     *  The cache entries removed are provided in the @param  itemlist.
     **/
    int  expireStale ( const time_t & now, std::list<ValueType> & itemlist )
    {
        CacheItem     *ci, *cr = NULL;
        TimerSetIter   tIter, rIter;
        
        tIter = _timers.begin();

        while ( tIter != _timers.end() && tIter->first < now ) {
            ci = tIter->second;

            if ( ci )
                itemlist.push_back(ci->getValue());
            
            rIter = tIter;
            ++tIter;
            _timers.erase(rIter);

            if ( ci ) {
                cr = this->_pt->remove(ci->getPrefix());
                if ( ci == cr )
                    delete ci;
            }
        }

        return itemlist.size();
    }

    /**  The number of entries in the cache. */
    size_t size() 
    {
        return this->_pt->size();
    }

    /**  Returns the current cache timeout interval in seconds */
    const time_t&  cacheTimeout() const
    {
        return _cacheTimeout;
    }

    /**  Sets the current cache timeout interval to the given value */
    void cacheTimeout ( time_t timeout_interval )
    {
        _cacheTimeout = timeout_interval;
    }


  private:

    CacheTree*          _pt;
    CacheTimerSet       _timers;
    time_t              _cacheTimeout;
    bool                _lock;
};


}  // namespace


#endif  //  _TCANETPP_PREFIXCACHE_HPP_

