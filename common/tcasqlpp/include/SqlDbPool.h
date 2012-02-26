/**  SqlDbPool.h
  *     Database connection pooling class.
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
#ifndef _TCASQLPP_SQLDBPOOL_H_
#define _TCASQLPP_SQLDBPOOL_H_

#include <string>
#include <list>

#include "ThreadLock.h"

#include "SqlSessionInterface.hpp"
#include "SqlFactoryInterface.hpp"


namespace tcasqlpp {


//defaults
#define TCASQL_DBPOOL_MIN   1
#define TCASQL_DBPOOL_DEF   50
#define TCASQL_DBPOOL_MAX   512


// hard limit to pool
#ifndef HARDMAX_DB_CONNS
#define HARDMAX_DB_CONNS   512
#endif


class SqlDbPool {


    typedef SqlFactoryInterface*  SQLFF;
    typedef SqlSessionInterface*  SQLSI;

    typedef std::list<SQLSI>      DbConnList;

  public:

    SqlDbPool();

    SqlDbPool ( SQLSI  master, SQLFF  factory );

    SqlDbPool ( const std::string & dbname, const std::string & dbhost,
                const std::string & dbuser, const std::string & dbpass,
                const std::string & dbport = "");

    virtual ~SqlDbPool();


    SQLSI        acquire();
    void         release    ( SQLSI  conn );

    bool         maxConnections ( int max );
    int          maxConnections() const;
    void         minConnections ( int min );
    int          minConnections() const;

    bool         SetSession ( SQLSI master, SQLFF factory );

    int          connsAvailable();
    int          connsInUse();
    int          connsCreated();
    int          flush();

    void         implicitLock ( bool implicit_lock = true );

    const
    std::string& getErrorStr() { return _sqlerr; }

    
  private:

    int          createInstances();


  protected:

    DbConnList            _dbin;
    DbConnList            _dbout;

    SQLSI                 _sqlmaster;
    SQLFF                 _sqlfactory;

    tcanetpp::ThreadLock  _mutex;

    bool                  _sync;

    std::string           _dbname;
    std::string           _dbhost;
    std::string           _dbuser;
    std::string           _dbpass;
    std::string           _dbport;
    std::string           _sqlerr;

    int                   _conncnt;
    int                   _mincnt;
    int                   _maxcnt;
    int                   _connavg;

};

} // namespace


#endif  /*  _TCASQLPP_SQLDBPOOL_H_  */
