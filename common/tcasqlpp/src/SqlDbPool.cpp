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
#include <sstream>

#include "SqlDbPool.h"
#include "SqlSessionFactory.hpp"

using namespace tcanetpp;


namespace tcasqlpp {



SqlDbPool::SqlDbPool()
    : _sqlmaster(NULL),
      _sqlfactory(NULL),
      _sync(false),
      _conncnt(0),
      _mincnt(TCASQL_DBPOOL_MIN),
      _maxcnt(TCASQL_DBPOOL_DEF),
      _connavg(0)
{}


SqlDbPool::SqlDbPool ( SqlSessionInterface * master, SqlFactoryInterface * factory )
    : _sqlmaster(master),
      _sqlfactory(factory),
      _sync(false),
      _conncnt(0),
      _mincnt(TCASQL_DBPOOL_MIN),
      _maxcnt(TCASQL_DBPOOL_DEF),
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
      _mincnt(TCASQL_DBPOOL_MIN),
      _maxcnt(TCASQL_DBPOOL_DEF),
      _connavg(0)
{
    _sqlfactory = new SqlSessionFactory();
    _sqlmaster  = (SqlSessionInterface*) new SqlSession(_dbname, _dbhost, 
                    _dbuser, _dbpass, _dbport);
}


SqlDbPool::~SqlDbPool()
{
    DbConnList::iterator  dIter;

    for ( dIter = _dbin.begin(); dIter != _dbin.end(); ++dIter ) {
        if ( *dIter )
            delete *dIter;
    }
    _dbin.clear();

    for ( dIter = _dbout.begin(); dIter != _dbout.end(); ++dIter ) {
        if ( *dIter )
            delete *dIter;
    }
    _dbout.clear();

    if ( _sqlmaster )
        delete _sqlmaster;
    if ( _sqlfactory )
        delete _sqlfactory;
}


bool
SqlDbPool::SetSession ( SqlSessionInterface * master,
                        SqlFactoryInterface * factory )
{
    ThreadAutoMutex       lock(&_mutex, _sync);
    DbConnList::iterator  dIter;

    if ( _dbout.size() > 0 )
        return false;

    if ( master ) {
        if ( _sqlmaster )
            delete _sqlmaster;

        _sqlmaster = master;

        for ( dIter = _dbin.begin(); dIter != _dbin.end(); ++dIter ) {
            if ( *dIter )
                delete *dIter;
        }
        _dbin.clear();
    }

    if ( factory ) {
        if ( _sqlfactory )
            delete _sqlfactory;
        _sqlfactory = factory;
    }

    return true;
}


SqlSessionInterface*
SqlDbPool::acquire()
{
    ThreadAutoMutex lock(&_mutex, _sync);
    SqlSessionInterface * conn = NULL;

    if ( _dbin.empty() )
    {
        if ( this->createInstances() == 0 ) {
            _sqlerr = "Cannot create any more instances";
            return conn;
        }
    }

    if ( ! _dbin.empty() ) {
        conn = _dbin.front();
        _dbin.pop_front();
    }

    if ( conn != NULL )
    {
        if ( ! conn->isConnected() && ! conn->dbconnect() ) {
            _sqlerr = conn->sqlErrorStr();
            conn->dbclose();
            _conncnt--;
            delete conn;
            return NULL;
        } else
            _dbout.push_front(conn);
    }
    else
    {
        std::ostringstream ostr;
        ostr << "SqlDbPool::acquire() no handles, "
             <<  _dbout.size() << " in use, " << _conncnt << " created.";
        _sqlerr.assign(ostr.str());
    }

    return conn;
}


void
SqlDbPool::release ( SqlSessionInterface * conn )
{
    ThreadAutoMutex lock(&_mutex, _sync);
    DbConnList::iterator  dIter;

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

    return;
}


int
SqlDbPool::connsAvailable()
{
    ThreadAutoMutex lock(&_mutex, _sync);
    return _dbin.size();
}

int
SqlDbPool::connsInUse()
{
    ThreadAutoMutex lock(&_mutex, _sync);
    return _dbout.size();
}

int
SqlDbPool::connsCreated()
{
    ThreadAutoMutex lock(&_mutex, _sync);
    return _conncnt;
}

bool
SqlDbPool::maxConnections ( int max )
{
    ThreadAutoMutex lock(&_mutex, _sync);

    if ( max >= TCASQL_DBPOOL_MAX )
        return false;

    if ( max < _mincnt )
        _mincnt = (int)(max * .05);
    _maxcnt = max;

    return true;
}

int
SqlDbPool::maxConnections() const
{
    return _maxcnt;
}


void
SqlDbPool::minConnections ( int min )
{
    ThreadAutoMutex lock(&_mutex, _sync);
    
    if ( min > _maxcnt ) 
        min = _maxcnt;

    _mincnt = min;

    if ( _conncnt < min )
        this->createInstances();
}

int          
SqlDbPool::minConnections() const
{
    return _mincnt;
}

 
int
SqlDbPool::createInstances()
{
    SqlSessionInterface * conn = NULL;

    int cur = (int) _dbin.size() + _dbout.size();

    if ( cur >= _maxcnt ) {
        _sqlerr = "SqlDbPool::create() max instances reached.";
        return 0;
    }
    
    if ( _sqlmaster == NULL  ) {
        _sqlerr = "SqlDbPool::create() invalid master session";
        return 0;
    }
    
    if ( _sqlfactory == NULL ) {
        _sqlerr = "SqlDbPool::create() using a default factory.";
        _sqlfactory = (SqlFactoryInterface*) new SqlSessionFactory();
    }

    int num = (int) ((_maxcnt - _mincnt) * .05);

    if ( num < _mincnt )
        num = _mincnt;
    else if ( num < 1 )
        num = _mincnt;

    if ( (_conncnt + num) > _maxcnt )
        num = _maxcnt - _conncnt;

    for ( int i = 0; i < num; i++ ) {
        conn =  (*_sqlfactory)(_sqlmaster);
        _dbin.push_front(conn);
        _conncnt++;
    }

    return num;
}

int
SqlDbPool::flush()
{
    int  cleared = 0;

    ThreadAutoMutex lock(&_mutex, _sync);

    if ( _dbin.size() > (u_int) _mincnt ) {
        SqlSessionInterface * conn = _dbin.front();
        _dbin.pop_front();
        if ( conn )
            delete conn;
        cleared++;
    }
    _conncnt -= cleared;

    return cleared;
}

void
SqlDbPool::implicitLock ( bool implicitLock )
{
    _sync = implicitLock;
}
    

} // namespace


//  _TCASQLPP_MYSQLDBPOOL_CPP_

