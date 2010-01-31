/* 
 * Jan 30 08:31:26 nebula kernel: FW: 
 * IN=ppp0 OUT= MAC= SRC=146.201.161.125 DST=75.214.27.0 LEN=39 TOS=0x00 PREC=0x00 TTL=113 ID=22852 PROTO=UDP SPT=10885 DPT=4324 LEN=19 
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

#include "FileUtils.h"
#include "StringUtils.h"
#include "EventManager.h"
using namespace tcanetpp;


#define DEFAULT_FWLOGKEY " kernel: FW: "
#define FWLOG_FIELDCOUNT 14


static const
char process[]    = "fwlogp";

struct FwEntry {
    std::string  date;
    std::string  inf;
    std::string  outf;
    std::string  src;
    std::string  dst;
    std::string  proto;
    std::string  spt;
    std::string  dpt;
};

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


void
parseFields ( std::vector<std::string> & fwv )
{
    std::vector<std::string>::iterator  vIter;

    std::string  equal = "=";
    int          indx  = 0;

    for ( vIter = fwv.begin(); vIter != fwv.end(); ++vIter ) 
    {
        std::string & field = *vIter;

        if ( (indx = StringUtils::indexOf(field, equal)) > 0 )
            if ( field.length() > ((u_int)indx + 1) )
                field.erase(0, indx+1);
    }

    return;
}



void 
parseFwLine ( const std::string & line, int indx, FwEntry & fwe )
{
    std::vector<std::string>  fwv;
    std::string               fwln, datehost;
    int                       fldcnt = FWLOG_FIELDCOUNT;
    
    fwln     = line;
    datehost = fwln.substr(0, indx);
    fwln.erase(0, indx);

    StringUtils::split(fwln, ' ', std::back_inserter(fwv));

    if ( fwv.size() < (u_int) fldcnt ) {
        std::cout << "Line: '" << fwln << "'" << std::endl
            << "  has " << fwv.size() << " fields, need " << fldcnt
            << std::endl;
    } else if ( fwv.size() > fldcnt ) { //udp match
        std::cout << "fwlog line match " << std::endl;


        parseFields(fwv);

        int i = 10;

        fwe.inf    = fwv[0];
        fwe.outf   = fwv[1];
        //mac  = 2
        fwe.src    = fwv[3];
        fwe.dst    = fwv[4];
        //fwe.len    = fwv[5];
        //tos  = 6
        //prec = 7
        //ttl  = 8
        //id   = 9
        if ( fwv.size() == 24 && fwv[i].compare("DF") == 0 )
            i++;
        fwe.proto  = fwv[i]; 
        i++;
        fwe.spt    = fwv[i];
        i++;
        fwe.dpt    = fwv[i];

        std::cout << " FW: IN=" << fwe.inf << " OUT=" << fwe.outf 
                  << " SRC=" << fwe.src << " DST=" << fwe.dst 
                  << " PROTO=" << fwe.proto  << " SPT=" << fwe.spt
                  << " DPT=" << fwe.dpt << std::endl;
    }

    fwv.clear();

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

    if ( logf != NULL && strlen(logf) > 0 ) 
    {
        logfile = logf;
        ::free(logf);
    }
    else
    {
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

    char         line[BIGSTRLINE];
    std::string  match = DEFAULT_FWLOGKEY;
    int          indx  = 0;

    while ( ifs.getline(line, BIGSTRLINE) )
    {
        if ( (indx = StringUtils::indexOf(line, match)) >= 0 ) 
        {
            FwEntry      fwe;
            parseFwLine(line, indx+match.length(), fwe);
        }
    }

    ifs.close();

    return 0;
}




