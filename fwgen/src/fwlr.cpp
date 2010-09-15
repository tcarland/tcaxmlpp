#define _FWGEN_FWLOGPARSE_CPP_

extern "C" {
# include <signal.h>
# include <unistd.h>
# include <sys/stat.h>
}

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

#include <vector>

#include "Thread.h"
#include "SynchronizedQueue.hpp"

#include "FileUtils.h"
#include "StringUtils.h"
#include "EventManager.h"
#include "LogFacility.h"
using namespace tcanetpp;

#include "FwLogEntry.h"
#include "FwLogReport.h"
#include "FwLogReader.h"
using namespace fwgen;




static const
char process[]    = "fwlr";
bool Alarm        = false;
char Version[]    = "v0.5.1";


void usage()
{
    printf("Usage: %s -dDht -c <apicfg> -l <logfile>\n", process);
    exit(0);
}

void version()
{
    printf("%s Version: %s\n", process, Version);
    exit(0);
}

void initDaemon ( const char* pname )
{
    pid_t  pid;

    if ( (pid = fork()) != 0 )
        exit(0);

    setsid();
    signal(SIGHUP, SIG_IGN);

    if ( (pid = fork()) != 0 )
        exit(0);
    chdir("/");
    umask(0);

    for ( int i = 1; i < MAX_FDVAL; i++ )
        close(i);

    return;
}


void sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM )
        Alarm = true;
    return;
}


int main ( int argc, char **argv )
{
    std::string logfile, config;
    char        optChar;
    char      * logf   = NULL;
    char      * cfg    = NULL;
    bool        debug  = false;
    bool        daemon = false;
    bool        tail   = false;

    if ( argc < 2 )
        usage();

    while ( (optChar = getopt(argc, argv, "c:dDhl:tV")) != EOF ) {
        switch ( optChar ) {
            case 'c':
            	cfg = strdup(optarg);
            	break;
            case 'd':
                debug = true;
                break;
            case 'D':
                daemon = true;
                break;
            case 'h':
                usage();
                break;
            case 'l':
                logf = strdup(optarg);
                break;
            case 't':
                tail = true;
                break;
            case 'V':
                version();
                break;
            default:
                std::cout << "wtf" << std::endl;
                usage();
                break;
        }
    }

    if ( logf != NULL ) {
        logfile = logf;
        ::free(logf);
    } else {
        //usage();
    }

    if ( cfg != NULL ) {
    	config = cfg;
    	::free(cfg);
    }

    if ( ! FileUtils::IsReadable(logfile.c_str()) ) {
        std::cout << "Error: File not found or is not readable: " 
                  << logfile << std::endl;
        return -1;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT,  &sigHandler);
    signal(SIGTERM, &sigHandler);

    LogFacility::InitThreaded(true);

    if ( daemon )
        LogFacility::OpenSyslog("fwlr", 5);
    else
        LogFacility::OpenLogStream("stdout", "fwlr", &std::cout);

    FwLogQueue * queue = NULL;

    time_t now;
    FwLogEntry  fwe;
    FwLogReport * fwrep    = NULL;
    FwLogReader * fwreader = new FwLogReader(logfile, tail);
    fwreader->start();

    queue = fwreader->getQueue();

    while ( ! Alarm )
    {

        now = ::time(NULL);

        if ( queue->pop(fwe) > 0 ) 
        {
            if ( fwrep == NULL ) {
            	if ( ! config.empty() ) {
            		fwrep = new FwLogReport(config);
                } else {
	                std::string  agent = "fwlr/";
	                agent.append(fwe.host);
	                fwrep = new FwLogReport(agent, "localhost", 15605);
                }
            }

            fwrep->SendEntry(fwe, now);
        } 
        else 
        {
            queue->waitFor(5);
            queue->unlock();
        }
        
        if ( fwrep )
            fwrep->FlushApi(now);        
            
        if ( ! tail && ! fwreader->isRunning() && queue->size() == 0 ) {
            fwreader->stop();
            delete fwreader;
            tail      = true;
            LogFacility::LogMessage("Full pass completed, switching to tail mode");
            fwreader  = new FwLogReader(logfile, tail);
            fwreader->start();
            queue = fwreader->getQueue();
        }

    }

    LogFacility::LogMessage("Finished.");
    if ( ! daemon )
        LogFacility::RemoveLogStream("stdout");
    LogFacility::CloseLogFacility();

    return 0;
}




