#ifndef _INCLUDE_SQLSESSIONFACTORY_HPP_
#define _INCLUDE_SQLSESSIONFACTORY_HPP_

#include "SqlFactoryInterface.hpp"
#include "SqlSession.hpp"

namespace tcasql {


/**  SqlSessionFactory is the 'default' factory used by SqlDbPool for 
  *  generating new db session instances. A new session is created by 
  *  cloning the provided instance, which should be of type SqlSession.
 **/
class SqlSessionFactory : public SqlFactoryInterface {
    
  public:
      
    SqlSessionFactory() {}
    virtual ~SqlSessionFactory() {}
    
    virtual SqlSessionInterface* operator() ( SqlSessionInterface * master )
    {
        SqlSession * session = (SqlSession*) master;
        return( (SqlSessionInterface*) new SqlSession(*session) );
    }

};


}  // namespace

#endif  // _INCLUDE_SQLSESSIONFACTORY_HPP_
