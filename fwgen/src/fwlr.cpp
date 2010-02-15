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

#include "FwLogEntry.h"
#include "FwLogReport.h"

#include "FileUtils.h"
#include "StringUtils.h"
#include "EventManager.h"
using namespace tcanetpp;


static const
char process[]    = "fwlogp";
bool Alarm        = false;


void usage()
{
    printf("Usage: %s -c <logfile>\n", process);
    exit(0);
}

void version()
{
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
    std::string logfile;
    char        optChar;
    char      * logf   = NULL;
    bool        debug  = false;
    bool        daemon = false;
    bool        tail   = false;

    if ( argc < 2 )
        usage();

    while ( (optChar = ::getopt(argc, argv, "c:dDhtV")) != EOF ) {
        switch ( optChar ) {
            case 'c':
                logf = ::strdup(optarg);
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
            case 't':
                tail = true;
                break;
            case 'V':
                version();
                break;
            default:
                usage();
                break;
        }
    }
    
    if ( logf != NULL && strlen(logf) > 0 ) {
        logfile = logf;
        ::free(logf);
    } else {
        usage();
    }

    if ( ! FileUtils::IsReadable(logfile.c_str()) ) {
        std::cout << "Error: File not found or is not readable: " 
                  << logfile << std::cout;
        return -1;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT,  &sigHandler);
    signal(SIGTERM, &sigHandler);

    std::ifstream        ifs;
    std::ios::openmode   mode = std::ios::in;

    if ( tail )
        mode |= std::ios::ate;

    ifs.open(logfile.c_str(), mode);
    if ( ! ifs ) {
        std::cout << "Error reading logfile ";
        return -1;
    }

    char   line[BIGSTRLINE];
    time_t now;

    FwLogReport *  fwrep = NULL;

    do {
        while ( ifs.getline(line, BIGSTRLINE) )
        {
            FwLogEntry   fwe;

            if ( FwLogEntry::ParseLogEntry(line, fwe) )
            {
                if ( fwrep == NULL ) {
                    std::string  agent = "fwgen/";
                    agent.append(fwe.host);

                    fwrep = new FwLogReport(agent);
                }
                now = ::time(NULL);

                fwrep->SendEntry(fwe, now);
            }
        }
        if ( fwrep )
            fwrep->FlushApi(now);
        sleep(2);
    } while ( ifs.eof() && ! Alarm );

    std::cout << "Finished." << std::endl;

    ifs.close();

    return 0;
}




