/**  SqlDbPool.cpp
  *    c++ wrapper to a MYSQL session instance.
  *  
  * Copyright (c) 2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
  *
  * This file is part of tcasqlpp.
  *
  * tcasqlpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcasqlpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcasqlpp.  
  * If not, see <http://www.gnu.org/licenses/>.
  *
 **/
#define _TCASQLPP_SQLDBPOOL_CPP_

#include <algorithm>

#include "SqlDbPool.h"
#include "SqlSessionFactory.hpp"


namespace tcasqlpp {


SqlDbPool::SqlDbPool()
    : _sqlmaster(NULL),
      _sqlfactory(NULL),
      _sync(false),
      _conncnt(0),
      _mincnt(MINIMUM_DB_CONNS),
      _maxcnt(DEFAULT_MAX_CONNS),
      _connavg(0)
{}


SqlDbPool::SqlDbPool ( SqlSessionInterface * master, SqlFactoryInterface * factory )
    : _sqlmaster(master),
      _sqlfactory(factory),
      _sync(false),
      _conncnt(0),
      _mincnt(MINIMUM_DB_CONNS),
      _maxcnt(DEFAULT_MAX_CONNS),
      _connavg(0)
{}


SqlDbPool::SqlDbPool ( const std::string & dbname, const std::string & dbhost,
                       const std::string & dbuser, const std::string & dbpass,
                       const std::string & dbport )
    : _sync(false),
      _dbname(dbname),
      _dbhost(dbhost),
      _dbuser(dbuser),
      _dbpass(dbpass),
      _dbport(dbport),
      _conncnt(0),
      _mincnt(MINIMUM_DB_CONNS),
      _maxcnt(DEFAULT_MAX_CONNS),
      _connavg(0)
{
    _sqlmaster  = (SqlSessionInterface*) new SqlSession(_dbname, _dbhost, 
                    _dbuser, _dbpass, _dbport);
    _sqlfactory = new SqlSessionFactory();
}


SqlDbPool::~SqlDbPool()
{
    if ( this->connsInUse() > 0 )
        _dbout.clear();  // leakage!

    DbConnList::iterator  dIter;

    for ( dIter = _dbin.begin(); dIter != _dbin.end(); ++dIter )
        if ( *dIter )
            delete *dIter;

    _dbin.clear();

    if ( _sqlmaster )
        delete _sqlmaster;
    if ( _sqlfactory )
        delete _sqlfactory;
}


void
SqlDbPool::SetSessionFactory ( SqlSessionInterface * master, 
                               SqlFactoryInterface * factory )
{
    if ( master ) {
        if ( _sqlmaster )
            delete _sqlmaster;
        _sqlmaster = master;
    }

    if ( factory ) {
        if ( _sqlfactory )
            delete _sqlfactory;
        _sqlfactory = factory;
    }

    return;
}


SqlSessionInterface*
SqlDbPool::acquire()
{
    SqlSessionInterface * conn = NULL;

    this->lock();
   
    if ( _dbin.empty() )
        this->createInstances();

    if ( ! _dbin.empty() ) {
        conn = _dbin.front();
        _dbin.pop_front();
    }

    if ( conn != NULL ) {
        if ( ! conn->isConnected() && ! conn->dbconnect() ) {
            _sqlerr = conn->sqlErrorStr();
            conn->dbclose();
            delete conn;
            conn = NULL;
        }
    } else {
        _sqlerr = "SqlDbPool::acquire() no more connections";
    }

    if ( conn )
        _dbout.push_front(conn);

    this->unlock();

    return conn;
}


void
SqlDbPool::release ( SqlSessionInterface * conn )
{
    DbConnList::iterator  dIter;

    this->lock();

    dIter = find(_dbout.begin(), _dbout.end(), conn);

    if ( dIter == _dbout.end() ) {
        if ( conn ) {
            conn->dbclose();
            delete conn;
        }
        
    } else {
        _dbout.erase(dIter);
        _dbin.push_front(conn);
    }

    _conncnt = _dbin.size() + _dbout.size();
    this->unlock();

    return;
}


int
SqlDbPool::connsAvailable()
{
    int cnt = 0;
    this->lock();
    cnt = _dbin.size();
    this->unlock();
    return(cnt);
}

int
SqlDbPool::connsInUse()
{
    int conns = 0;
    this->lock();
    conns = _dbout.size();
    this->unlock();
    return conns;
}

int
SqlDbPool::connsCreated()
{
    int conns = 0;
    this->lock();
    conns = _conncnt;
    this->unlock();
    return(conns);
}

bool
SqlDbPool::maxConnections ( int max )
{
    bool  result  = false;

    this->lock();
    if ( max <= HARDMAX_DB_CONNS ) 
    {
        if ( max < _mincnt )
            _mincnt = max;
        _maxcnt = max;
        result  = true;
    }
    this->unlock();

    return result;
}

int
SqlDbPool::maxConnections() const
{
    return _maxcnt;
}

void
SqlDbPool::minConnections ( int min )
{
    this->lock();
    
    if ( min > _maxcnt ) 
        min = _maxcnt;
    if ( _conncnt < min ) {
        this->createInstances();
    }
    _mincnt = min;

    this->unlock();
}

int          
SqlDbPool::minConnections() const
{
    return _mincnt;
}

 
void
SqlDbPool::createInstances()
{
    SqlSessionInterface * conn = NULL;

    if ( _conncnt >= _maxcnt )
        return;
    
    if ( _sqlmaster == NULL  ) {
        _sqlerr = "SqlDbPool::createInstances() invalid master session";
        return;
    }
    
    if ( _sqlfactory == NULL ) {
        _sqlerr = "SqlDbPool::createInstances():invalid factory, using default";
        _sqlfactory = (SqlFactoryInterface*) new SqlSessionFactory();
    }

    int num = (int) ((_maxcnt - _mincnt) * .05) / 2;

    if ( num > _mincnt )
        num = _mincnt;
    else if ( num < 1 )
        num = 1;

    if ( (_conncnt + num) > _maxcnt )
        num = _maxcnt - _conncnt;

    for ( int i = 0; i < num; i++ ) {
        conn =  (*_sqlfactory)(_sqlmaster);
        _dbin.push_front(conn);
        _conncnt++;
    }

    return;
}

int
SqlDbPool::flush()
{
    int  cleared = 0;

    this->lock();

    if ( _dbin.size() > (u_int) _mincnt ) {
        SqlSessionInterface * conn = _dbin.front();
        _dbin.pop_front();
        if ( conn )
            delete conn;
        cleared++;
    }
    _conncnt -= cleared;

    this->unlock();

    return cleared;
}

bool
SqlDbPool::implicitLock ( bool implicitLock )
{
#   ifdef WIN32
    return false;
#   else
    _sync = implicitLock;
#   endif 
    return true;
}
    
void
SqlDbPool::lock()
{
#ifndef WIN32
    if ( this->_sync )
        this->_mutex.lock();
#endif
    return;
}

void
SqlDbPool::unlock()
{
#ifndef WIN32
    if ( this->_sync )
        this->_mutex.unlock();
#endif
    return;
}


} // namespace


//  _TCASQLPP_MYSQLDBPOOL_CPP_

