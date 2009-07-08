#define _SOURCE_TESTMYSQLSESSION_CPP_

#include "TestMysqlSession.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace sqlplustest {


SqlSessionInterface*
TestSqlFactory::operator() ( SqlSessionInterface * master )
{
    TestMysqlSession * session = (TestMysqlSession*) master;
    return( (SqlSessionInterface*) new TestMysqlSession(*session) );
}


TestMysqlSession::TestMysqlSession()
    : MysqlSession()
{}


TestMysqlSession::TestMysqlSession ( const std::string & dbhost,
                                     const std::string & dbname,
                                     const std::string & dbuser,
                                     const std::string & dbpass,
                                     const std::string & dbport )
    : MysqlSession(dbhost, dbname, dbuser, dbpass, dbport)
{}


TestMysqlSession::TestMysqlSession ( const TestMysqlSession & session )
    : MysqlSession((MysqlSession)session)
{}


TestMysqlSession::~TestMysqlSession() {}


bool
TestMysqlSession::printQuery ( const std::string & sql, 
                               const std::string & strmname )
{
    Query  query = this->newQuery();
    Result res;
    Row    row;

    Result::iterator  rIter;

    query << sql;
    LogFacility::LogToStream(strmname, "printQuery(): " + query.preview());

    if ( ! this->submitQuery(query, res) ) {
        LogFacility::LogToStream(strmname, "Error submitting query: " 
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

        LogFacility::LogToStream(strmname, rowstr);
    }

    return true;
}


}  // namespace

