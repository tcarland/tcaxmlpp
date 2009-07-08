#define _SOURCE TESTSQLTHREAD_CPP_

#include <fstream>

#include "TestSqlThread.h"
#include "TestSqlSession.hpp"


#include "LogFacility.h"
using namespace tcanetpp;


namespace sqlplustest {


TestSqlThread::TestSqlThread ( SqlDbPool * pool, const std::string & sql )
    : Thread(),
      _pool(pool),
      _sqlstr(sql)
{
}


TestSqlThread::~TestSqlThread() {}


void
TestSqlThread::run()
{
    TestSqlSession * session = NULL;

    std::string  logfile = this->_threadName;
    logfile.append(".log");
    
    LogFacility::AddLogStream(logfile, "TestSqlThread",
            (std::ostream*) new std::ofstream(logfile.c_str()));
    LogFacility::LogToStream(logfile, "Thread starting: " + _threadName);

    // we implement a block on acquiring a db handle here, but 
    // can choose to error after num of attempts or time 
    while ( ! _Alarm && session == NULL ) {
        LogFacility::LogToStream(logfile, "Trying to get db instance: " + _threadName);
        
        session = (TestSqlSession*) _pool->acquire();
        
        if ( session == NULL ) {
            
            if ( ! _pool->getErrorStr().empty() ) {
                LogFacility::LogToStream(logfile, "Error obtaining session: " 
                    + _pool->getErrorStr());
                break;
            }

            LogFacility::LogToStream(logfile, "Thread blocking on db instance: "
                    + _threadName);
            sleep(3);
        }
    }

    if ( _Alarm || session == NULL )
        return;
    
    session->printQuery(_sqlstr, logfile);

    LogFacility::LogToStream(logfile, "Releasing thread: " + _threadName);
    _pool->release(session);
    
    LogFacility::LogToStream(logfile, "Thread finished: " + _threadName);
    std::ofstream* fstrm = (std::ofstream*) LogFacility::RemoveLogStream(logfile);
    delete fstrm;

    return;
}


} // namespace


