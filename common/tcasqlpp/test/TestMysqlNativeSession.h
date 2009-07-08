#ifndef _INCLUDE_TESTMYSQLNATIVESESSION_H_
#define _INCLUDE_TESTMYSQLNATIVESESSION_H_


#include "MysqlNativeSession.h"
#include "SqlFactoryInterface.hpp"
using namespace tcasqlpp;


namespace sqlplustest {
   
/**  TestSqlFactory for generating the appropriate TestSqlSession */
class TestSqlFactory : public SqlFactoryInterface {
  public:
    virtual ~ TestSqlFactory() {}
    virtual SqlSessionInterface* operator() ( SqlSessionInterface * master );
};


class TestMysqlNativeSession : public MysqlNativeSession {

  public:

    TestMysqlNativeSession();
    TestMysqlNativeSession ( const std::string & dbhost,
                             const std::string & dbname,
                             const std::string & dbuser,
                             const std::string & dbpass,
                             const std::string & dbport = "" );
    
    TestMysqlNativeSession ( const TestMysqlNativeSession & session );

    virtual ~TestMysqlNativeSession();


    bool  printQuery ( const std::string & sql, 
                       const std::string & strmname );

};

} // namespace

#endif // _INCLUDE_TESTMYSQLNATIVESESSION_H_
