#ifndef _TCASQL_PGSQLSESSION_H_
#define _TCASQL_PGSQLSESSION_H_

#include <string>

#include <pqxx/pqxx>
using namespace pqxx;

#include "SqlSessionInterface.hpp"


namespace tcasqlpp {


class PgsqlSession : public SqlSessionInterface {

  public:

    PgsqlSession();
    PgsqlSession ( const std::string & dbname, const std::string & dbhost, 
                   const std::string & dbuser, const std::string & dbpass,
                   const std::string & dbport = "" );

    PgsqlSession ( const PgsqlSession & sql );

    virtual ~PgsqlSession();


    /* SqlSessionInterface */

    bool           connect()   { return this->dbconnect(); }
    virtual bool   dbconnect();
    virtual bool   dbconnect   ( const std::string & dbname, 
                                 const std::string & dbhost, 
                                 const std::string & dbuser, 
                                 const std::string & dbpass,
                                 const std::string & dbport = "" );
    
    void           close()     { return this->dbclose(); }
    virtual void   dbclose();

    bool           connected() { return this->isConnected(); }
    virtual bool   isConnected();

    std::string    escapeString ( const std::string & str )
    {
        return sqlesc(str);
    }

    virtual const
    std::string&   sqlErrorStr();


    /*  PgsqlSession methods */

    template<typename TRANSACTOR>
    bool           run         ( const TRANSACTOR & transaction ) const;


  protected:

    static std::string MakeConnStr ( const std::string & dbname, 
                                     const std::string & dbhost, 
                                     const std::string & dbuser, 
                                     const std::string & dbpass,
                                     const std::string & dbport );
  protected:

    pqxx::connection*   _dbcon;

    std::string         _connstr;
    std::string	        _sqlerr;

};

}  // namespace


#endif  //  _TCASQL_PGSQLSESSION_H_ 
