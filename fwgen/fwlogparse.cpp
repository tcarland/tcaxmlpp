/* 
  *
     fwlog /
       numOfEntries = 1000
       ignoreDest = false
       host / interface / src / dst / proto / dstport / count

 *
 */
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

#include "FileUtils.h"
#include "StringUtils.h"
#include "EventManager.h"
using namespace tcanetpp;



static const
char process[]    = "fwlogp";


void usage()
{
    printf("Usage: %s <logfile>", process);
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



int main ( int argc, char **argv )
{
    std::string logfile;
    char        optchar;
    char      * logf   = NULL;
    bool        daemon = false;
    bool        tail   = false;

    if ( argc < 2 )
        usage();
    
    logf = ::strdup(argv[1]);
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

    while ( ifs.getline(line, BIGSTRLINE) )
    {
        FwLogEntry      fwe;

        if ( FwLogEntry::ParseLogEntry(line, fwe) )
            std::cout << "SUBMIT" << std::endl;
    }

    ifs.close();

    return 0;
}




