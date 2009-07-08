#define _SOURCE_TESTMYSQLNATIVESESSION_CPP_

#include "TestMysqlNativeSession.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace sqlplustest {


SqlSessionInterface*
TestSqlFactory::operator() ( SqlSessionInterface * master )
{
    TestMysqlNativeSession * session = (TestMysqlNativeSession*) master;
    return( (SqlSessionInterface*) new TestMysqlNativeSession(*session) );
}


TestMysqlNativeSession::TestMysqlNativeSession()
    : MysqlNativeSession()
{}


TestMysqlNativeSession::TestMysqlNativeSession ( const std::string & dbhost,
                                                 const std::string & dbname,
                                                 const std::string & dbuser,
                                                 const std::string & dbpass,
                                                 const std::string & dbport )
    : MysqlNativeSession(dbhost, dbname, dbuser, dbpass, dbport)
{}


TestMysqlNativeSession::TestMysqlNativeSession ( const TestMysqlNativeSession & session )
    : MysqlNativeSession((MysqlNativeSession)session)
{}


TestMysqlNativeSession::~TestMysqlNativeSession() {}


bool
TestMysqlNativeSession::printQuery ( const std::string & sql, 
                               const std::string & strmname )
{
    Query  query = this->newQuery();
    Result res;
    Row    row;

    Result::iterator  rIter;

    query << sql;
    Logger::LogToStream(strmname, "printQuery(): " + query.preview());

    if ( ! this->submitQuery(query, res) ) {
        Logger::LogToStream(strmname, "Error submitting query: " 
                + this->sqlErrorStr());
        return false;
    }
    
    for ( rIter = res.begin(); rIter != res.end(); ++rIter ) {
        row = (Row) *rIter;
        std::string rowstr = "| ";

        for ( u_int f = 0; f < row.size(); ++f ) {
            std::string fld = (std::string) row[f];
            rowstr.append(" | ").append(fld);
        }

        Logger::LogToStream(strmname, rowstr);
    }

    return true;
}


}  // namespace

