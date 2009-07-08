#ifndef _TCASQL_SQLDBPOOL_H_
#define _TCASQL_SQLDBPOOL_H_

#include <string>
#include <list>

#ifndef WIN32
#include "ThreadLock.h"
#endif 


#include "SqlSessionInterface.hpp"
#include "SqlFactoryInterface.hpp"


namespace tcasqlpp {


//defaults
#define MINIMUM_DB_CONNS   1
#define MAXIMUM_DB_CONNS   100
#define DEFAULT_MAX_CONNS  30



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


    void         SetSessionFactory ( SQLSI  master, SQLFF  factory );
    
    SQLSI        acquire();
    void         release           ( SQLSI  conn );

    void         maxConnections    ( int max );
    int          maxConnections() const;
    void         minConnections    ( int min );
    int          minConnections() const;

    int          connsAvailable();
    int          connsInUse();
    int          connsCreated();
    int          flush();

    bool         implicitLock ( bool implicit_lock = true );

    const
    std::string& getErrorStr() { return _sqlerr; }
    
  private:

    void         createInstances();

    void         lock();
    void         unlock();

  protected:

    DbConnList            _dbin;
    DbConnList            _dbout;

    SQLSI                 _sqlmaster;
    SQLFF                 _sqlfactory;

#   ifndef WIN32
    tcanetpp::ThreadLock  _mutex;
#   endif
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


#endif  /*  _TCASQL_SQLDBPOOL_H_  */
