#define _FWGEN_FWLOGPARSE_CPP_

extern "C" {
# include <signal.h>
# include <unistd.h>
# include <sys/stat.h>
}
#include <getopt.h>

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
char Version[]    = "v0.5.2";


void usage()
{
    std::cout << "Usage: " << process << " [-dDhtV] -c <apicfg> -l <logfile>" << std::endl
              << "   -c | --config  <apicfg>  : Tnms API xml configuration" << std::endl
              << "   -d | --debug             : Enable debug output " << std::endl
              << "   -D | --daemon            : Run as daemon" << std::endl
              << "   -t | --tail              : Start reading of logfile at the tail" << std::endl
              << "   -m | --match             : Set the log match string" << std::endl
              << "   -l | --logfile <logfile> : The name of the logfile to parse" << std::endl
              << "   -V | --version           : Print version info and exit" << std::endl;
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
    std::string logfile, config, match;
    char        optChar;
    char      * logf    = NULL;
    char      * matchc  = NULL;
    char      * cfg     = NULL;
    bool        debug   = false;
    bool        daemon  = false;
    bool        tail    = false;
    int         optindx = 0;


    static struct option l_opts[] = { {"debug", no_argument, 0, 'd'},
                                      {"config", required_argument, 0, 'c'},
                                      {"daemon", no_argument, 0, 'D'},
                                      {"help", no_argument, 0, 'h'},
                                      {"logfile", required_argument, 0, 'l'},
                                      {"match", required_argument, 0, 'm'},
                                      {"tail", no_argument, 0, 't'},
                                      {"version", no_argument, 0, 'V'},
                                      {0,0,0,0}
                                    };

    if ( argc < 2 )
        usage();

    while ( (optChar = getopt_long(argc, argv, "c:dDhl:m:tV", l_opts, &optindx)) != EOF ) {
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
            case 'm':
                matchc = ::strdup(optarg);
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

    if ( matchc != NULL ) {
        match = matchc;
        ::free(matchc);
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
 
    time_t        now;
    FwLogEntry    fwe;
    FwLogQueue  * queue    = NULL;
    FwLogReport * fwrep    = NULL;
    FwLogReader * fwreader = new FwLogReader(logfile, tail);

    if ( ! match.empty() )
        fwreader->setMatch(match);

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
        
        // TODO: clean this up
        if ( ! tail && ! fwreader->isRunning() && queue->size() == 0 ) 
        {
            LogFacility::LogMessage("Full pass completed, switching to tail mode");
            fwreader->stop();
            delete fwreader;
            tail      = true;
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




