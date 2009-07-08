#define _sqlplustest_cpp_


extern "C" {
#include <unistd.h>
#include <signal.h>
}

#include <iostream>
#include <sstream>
#include <list>

#include "XmlDocument.h"
using namespace tcaxmlpp;

#include "LogFacility.h"
using namespace tcanetpp;

#include "SqlSession.hpp"
using namespace tcasqlpp;

#include "TestSqlThread.h"
#include "TestSqlSession.hpp"
using namespace sqlplustest;


struct TestConfig
{
    std::string  logfile;
    std::string  dbhost;
    std::string  dbname;
    std::string  dbuser;
    std::string  dbpass;
    std::string  dbport;
};


const char* process   = "sqlplustest";
bool        Alarm     = false;



void usage()
{
    printf("Usage: %s [-c cfg] [-n num] [-s sql]\n", process);
    printf("   -c cfg   : xml config file\n");
    printf("   -n num   : number of threads to generate\n");
    printf("   -s sql   : sql string to execute(should be quoted)\n");
    return;
}


void sigHandler ( int signal )
{
    printf("Caught signal %d\n", signal);
    if ( signal == SIGINT || signal == SIGTERM )
        Alarm = true;

    return;
}


bool parseConfig ( const std::string & cfgfile, TestConfig & config )
{
    XmlDocument  doc;

    if ( ! doc.initDocument(cfgfile) )
        return false;

    XmlNode * root = doc.getRootNode();

    config.logfile = root->getAttr("logfilename");
    config.dbhost  = root->getAttr("dbhost");
    config.dbname  = root->getAttr("dbname");
    config.dbuser  = root->getAttr("dbuser");
    config.dbpass  = root->getAttr("dbpass");
    config.dbport  = root->getAttr("dbport");

    return true;
}

int main ( int argc, char **argv )
{
    std::string  cfgfile, sqlStr;
    char         optChar;
    char        *cfgstr = NULL;
    char        *sqlstr = NULL;
    TestConfig   config;

    bool threadsRunning  = true;
    int  numOfThreads    = 5;
    
    while ( (optChar = getopt(argc, argv, "c:n:s:")) != EOF ) {
        
        switch ( optChar ) {
            case 'c':
              cfgstr = strdup(optarg);
              break;
            case 'n':
              numOfThreads = atoi(optarg);
              break;
            case 's':
              sqlstr = strdup(optarg);
              break;
            default:
              usage();
              break;
        }
    }

    if ( cfgstr != NULL ) {
        cfgfile = cfgstr;
        free(cfgstr);
    } else {
        usage();
        exit(0);
    }

    if ( sqlstr != NULL ) {
        sqlStr = sqlstr;
        free(sqlstr);
    } else {
        usage();
        exit(0);
    }
    printf("SQL: '%s'\n", sqlStr.c_str());

    if ( numOfThreads == 0 )
        numOfThreads = 1;

    if ( ! parseConfig(cfgfile, config) ) {
        printf("Failed to parse config");
        exit(0);
    }

    LogFacility::InitThreaded();
    LogFacility::AddLogStream("stdout", "sqlplustest", (std::ostream*) &std::cout);

    signal(SIGINT,  &sigHandler);
    signal(SIGTERM, &sigHandler);

    std::list<TestSqlThread*>  threads;
    std::list<TestSqlThread*>::iterator  tIter;

    SqlDbPool * dbpool;
    TestSqlSession * master;

    printf ("db params: dbhost %s : dbname %s : dbuser %s : dbpass %s : dbport %s\n",
            config.dbhost.c_str(), config.dbname.c_str(), config.dbuser.c_str(),
            config.dbpass.c_str(), config.dbport.c_str());

    master = new TestSqlSession(config.dbname, config.dbhost, 
             config.dbuser, config.dbpass, config.dbport);
    dbpool = new SqlDbPool((SqlSessionInterface*) master, 
                (SqlFactoryInterface*) new TestSqlFactory());
    dbpool->implicitLock();

    std::string threadName = "Thread";

    printf("Starting threads\n");
    for ( int i = 0; i < numOfThreads; ++i ) {
        std::ostringstream  tname;
        tname << threadName << i;

        TestSqlThread * thread = new TestSqlThread(dbpool, sqlStr);
        thread->threadName(tname.str());
        thread->start();
        threads.push_back(thread);
    }
    
    while ( ! Alarm && threadsRunning ) {
        for ( tIter = threads.begin(); tIter != threads.end();  ) {
            if ( (*tIter)->isRunning() ) {
                tIter++;
                continue;
            }

            printf("Clearing thread: '%s'\n",
                (*tIter)->threadName().c_str());
            delete *tIter;
            threads.erase(tIter++);
        }

        if ( threads.size() == 0 )
            threadsRunning = false;
        else {
            int inuse = dbpool->connsInUse();
            int avail = dbpool->connsAvailable();
            printf("Threads still running: %d\n", threads.size());
            printf("Threads running: %d, Pool connsInUse: %d connsAvail %d\n",
                threads.size(), inuse, avail);
        }

        sleep(1);
    }

    for ( tIter = threads.begin(); tIter != threads.end(); ++tIter ) {
        TestSqlThread * thread = *tIter;

        if ( thread == NULL )
            continue;

        printf("Attempting to signal: %s\n", thread->threadName().c_str());
        thread->setAlarm();
        sleep(1);
        delete thread;
    }
    threads.clear();

    printf("All threads finished\n");
    LogFacility::RemoveLogStream("stdout");

    delete dbpool;

    return 0;
} 


