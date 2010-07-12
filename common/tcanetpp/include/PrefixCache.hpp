/**  $Id: PrefixCache.hpp,v 1.9 2003/10/03 21:27:19 tca Exp $
  *
  *   PrefixCache
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
#ifndef _TCANETPP_PREFIXCACHE_HPP_
#define _TCANETPP_PREFIXCACHE_HPP_

#include <set>
#include <list>

#include "PrefixTree.hpp"


namespace tcanetpp {


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

    void            timeout ( int val )  { _expire = timeout; }
    int             timeout()            { return _expire; }

    const Prefix&   getPrefix()          { return _prefix; }
    Timer           getTimer()           { return _timer; }
    ValueType       getValue()           { return _value; }

    void            setTimer ( Timer t ) { this->_timer = t; }


  private:

    Prefix         _prefix;
    ValueType      _value;
    Timer          _timer;
    int            _expire;
};


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


    ValueType remove ( Prefix & p )
    {
        ValueType val;

        CacheItem * ci = this->_pt->exactMatch(p);

        if ( ci == NULL )
            return val;
        if ( ci.getPrefix() != p )
            return val;

        val = ci->getValue();
        ci  = this->_pt->remove(p);
        if ( ci )
            delete ci;

        return val;
    }


    ValueType match  ( Prefix & p )
    {
        ValueType  val;

        CacheItem * ci = this->_pt->exactMatch(p);

        if ( ci == NULL )
            return val;

        return ci->getValue();
    }


    bool refresh ( Prefix & p, time_t & now )
    {
    	CacheItem * ci  = this->match(p);

    	if ( NULL == ci || ci.getPrefix().getPrefix() == 0 )
            return false;

        TimerSetIter  timer  = ci->getTimer();
    	ci->setExpireTime(now + _cacheTimeout);
    	_timers.erase(timer);
    	timer = _timers.insert(CachePair(ci->getExpireTime(), ci));
    	ci->setTimer(timer);

    	return true;
    }


    int  expireStale ( const time_t & now, std::list<ValueType> & itemlist )
    {
        CacheItem       *ci, *cr;
        TimerSetIter     tIter = _timers.begin();

        while ( tIter != _timers.end() && tIter->first < now ) {
            ci = tIter->second;

            itemlist.push_back(ci->getValue());
            
            TimerSetIter  rIter = tIter;
            ++tIter;
            _timers.erase(rIter);
            cr = this->_pt->remove(ci->getPrefix());
            if ( cr == ci )
                delete ci;
        }

        return itemlist.size();
    }

    size_t size() 
    {
        return this->_pt->size();
    }

    const time_t&  cacheTimeout() const
    {
        return _cacheTimeout;
    }

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

