#ifndef _INCLUDE_TESTMYSQLSESSION_H_
#define _INCLUDE_TESTMYSQLSESSION_H_


#include "MysqlSession.h"
#include "SqlFactoryInterface.hpp"
using namespace tcasql;


namespace sqlplustest {
   
/**  TestSqlFactory for generating the appropriate TestSqlSession */
class TestSqlFactory : public SqlFactoryInterface {
  public:
    virtual ~ TestSqlFactory() {}
    virtual SqlSessionInterface* operator() ( SqlSessionInterface * master );
};


class TestMysqlSession : public MysqlSession {

  public:

    TestMysqlSession();
    TestMysqlSession ( const std::string & dbhost,
                       const std::string & dbname,
                       const std::string & dbuser,
                       const std::string & dbpass,
                       const std::string & dbport = "" );
    
    TestMysqlSession ( const TestMysqlSession & session );

    virtual ~TestMysqlSession();


    bool  printQuery ( const std::string & sql, 
                       const std::string & strmname );

};

} // namespace

#endif // _INCLUDE_TESTMYSQLSESSION_H_
