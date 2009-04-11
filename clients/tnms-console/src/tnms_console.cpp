

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <list>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/times.h>
#endif


#include "TnmsApi/TnmsAPI.h"




void  displayHelp()
{
    std::cout << std::endl;
}



int  runConsole ( std::istream & istrm, bool showprompt, bool echo = false )
{
    std::string  cmd, name, cfgfile, agentname, desc, timestr;
    std::string  prompt = "[tnms]: ";

    int     val;
    time_t  ts;
    size_t  maxlen = 1024;
    char    line[1024];

    if ( ! istrm ) 
        return -1;

    if ( showprompt )
        std::cout << prompt;

    while ( ! done && istrm.getline(line, len) )
    {
        if ( echo )
            std::cout << line << std::endl;

        if ( line[0] == '#' )
            continue;

        //switch ( cmd ) ?
        if ( cmd.compare("create") == 0 )
        {
            // create instance
            //
        }
        else if ( cmd.compare("quit") == 0 ) 
        {
            done = true;
            break;
        }
        else 
        {
            std::cout << "Unrecognized command: '" << cmd << "'" << std::endl;
        }

        if ( ! istrm.good() )
        {
            if ( showprompt )
                istrm.clear();
            else
                done = true;
        }

        if ( showprompt )
            std:: cout << prompt;
    }

    return 0;
}


int main ( int argc, char ** argv )
{
    std::list<std::string>  fileslist;
    std::list<std::string>::iterator  fIter;

    bool  showprompt = true;

    if ( fileslist.empty() )
        runConsole(std::cin, showprompt);
    else
    {
        for ( fIter = fileslist.begin(); fIter != fileslist.end(); ++fIter ) 
        {
            std::ifstream  fstrm;
            fstrm.open(fIter->c_str());
            if ( fstrm.is_open() ) {
                runConsole(fstrm, false, true);
                fstrm.close();
            } else
                std::cout << "Error reading file '" << *fIter << "'" << std::endl;
        }
    }

    return 0;
}

