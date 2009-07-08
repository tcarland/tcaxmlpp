/**  MysqlSession.h
  *   sqlplus wrapper to a MYSQL session instance.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  1.6
 **/
#ifndef _TCASQL_MYSQLSESSION_H_
#define _TCASQL_MYSQLSESSION_H_

#include <time.h>

#include <list>
#include <vector>
#include <string>
#include <sstream>

#include "mysql.h"

#include "SqlSessionInterface.hpp"
#include "SqlException.hpp"


namespace tcasqlpp {


#define DEFAULT_CONNECT_TIMEOUT 30


/**  Row  is an ordered list of column data */
typedef std::vector<std::string>   Row;

/**  Result is an ordered list of Row data */
typedef std::list<Row>             Result;


/**  Query is simply a convenient std::stringstream 
  *  used to construct sql statements.
 **/
class Query : public std::stringstream {
  public:
    Query() {}
    
    Query ( const Query & q )
        : std::stringstream(const_cast<Query&>(q).str())
    {}

    virtual ~Query() {}

    void         reset()   { std::stringstream(""); }
    std::string  preview() { return this->str(); }
};



/**  An instance of this class represents a single Mysql database
  *  connection.  This is an abstract class that can be either 
  *  inherited or used by any object wanting database access.
 **/ 
class MysqlSession : public SqlSessionInterface {

  public:

    MysqlSession() 
        throw ( SqlException );
    
    MysqlSession ( const std::string & dbname, const std::string & dbhost, 
                   const std::string & dbuser, const std::string & dbpass,
                   const std::string & dbport = "" )
        throw ( SqlException );
    
    MysqlSession ( const MysqlSession & sql ) 
        throw ( SqlException );

    virtual ~MysqlSession();


    /* SqlSessionInterface */

    bool           connect()   throw ( SqlException ) 
                               { return this->dbconnect(); }

    virtual bool   dbconnect() throw ( SqlException );

    virtual bool   dbconnect   ( const std::string & dbname, 
                                 const std::string & dbhost, 
                                 const std::string & dbuser, 
                                 const std::string & dbpass,
                                 const std::string & dbport = "" )
        throw ( SqlException );
    
    void           close()     { return this->dbclose(); }
    virtual void   dbclose();

    bool           connected() { return this->isConnected(); }
    virtual bool   isConnected();


    /*  Specific to MysqlSession */

    virtual Query  newQuery();
    virtual bool   submitQuery ( Query & q );
    virtual bool   submitQuery ( Query & q, Result & res );
    virtual bool   submitQuery ( const std::string & sql );

    uint64_t       insert_id();
    long           affected_rows();
    std::string    statement_info();
    std::string    character_set_name();

    uint32_t       server_version();
    uint32_t       client_version();

    void           timeout     ( time_t timeout );
    time_t         timeout();
    void           compression ( bool use_compression );
    bool           compression();

    void           setAutoReconnect ( bool auto_reconnect );
    bool           setCharacterSet  ( const std::string & charset_name );

    virtual const
    std::string&   sqlErrorStr();


    void           escapeString  ( std::ostringstream & q, 
                                   const std::string  & str );
    void           escapeString  ( Query & q, const std::string & str );
    std::string    escapeString  ( const std::string & str );


    template<typename T>
    static inline std::string toString ( const T & a )
    {
        std::stringstream  stream;
        stream << a;
        return stream.str();
    }
    
    template<typename T>
    static inline T  fromString ( const std::string & str )
    {
        T target = T();
        std::stringstream stream(str);
        stream >> target;
        return target;
    }

  private:

    void  init() throw ( SqlException );

  protected:

    MYSQL*		_dbcon;

    std::string	        _dbname;
    std::string	        _dbhost;
    std::string	        _dbuser;
    std::string	        _dbpass;
    std::string         _dbport;

    std::string	        _sqlerr;

    time_t              _timeout;
    bool                _compress;
    bool                _connected;

};

}  // namespace


#endif  //  _TCASQL_MYSQLSESSION_H_  
