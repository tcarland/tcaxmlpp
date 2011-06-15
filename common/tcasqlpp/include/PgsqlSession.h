/**  PgsqlSession.h
  *    c++ wrapper to a Postgres session instance.
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
#ifndef _TCASQLPP_PGSQLSESSION_H_
#define _TCASQLPP_PGSQLSESSION_H_

#include <string>

#include <pqxx/pqxx>
using namespace pqxx;

#include "SqlSessionInterface.hpp"


namespace tcasqlpp {


class PgsqlSession : public SqlSessionInterface {

  public:

    PgsqlSession();
    PgsqlSession ( const std::string & dbname,
                   const std::string & dbuser,
                   const std::string & dbpass,
                   const std::string & dbhost,
                   const std::string & dbport = "" );

    PgsqlSession ( const PgsqlSession & sql );

    virtual ~PgsqlSession();


    /* SqlSessionInterface */

    bool           connect()   { return this->dbconnect(); }
    virtual bool   dbconnect();
    virtual bool   dbconnect   ( const std::string & dbname, 
                                 const std::string & dbuser, 
                                 const std::string & dbpass,
                                 const std::string & dbhost,
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
                                     const std::string & dbuser, 
                                     const std::string & dbpass,
                                     const std::string & dbhost,
                                     const std::string & dbport );
  protected:

    pqxx::connection*   _dbcon;

    std::string         _connstr;
    std::string	        _sqlerr;

};

}  // namespace


#endif  //  _TCASQLPP_PGSQLSESSION_H_ 
