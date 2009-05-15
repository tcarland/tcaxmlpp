
extern "C" {
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
}

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/times.h>
#endif


#include "TnmsAPI.h"
using namespace tnmsCore;

#include "LogFacility.h"
using namespace tcanetpp;


typedef std::map<std::string, TnmsAPI*>             ApiMap;
typedef std::map<std::string, TnmsAPI*>::iterator   ApiIter;
typedef std::pair<ApiIter, bool>                    ApiMapInsert;

bool    connection = false;


void  displayHelp()
{
    std::cout << std::endl;

    std::cout << " tnms_console - a command-line agent process " << std::endl;
    std::cout << "Usage: tnms-console [-noprompt] [script file]" << std::endl;

    std::cout << std::endl;

    std::cout << "TnmsAPI Instance commands:" << std::endl;
    std::cout << " create [tag] [agent-name] [configfile]  =  Creates a new instance." << std::endl;
    std::cout << " destroy [tag]                           =  Destroys an instance." << std::endl;
    std::cout << " list                                    =  Lists available instances." << std::endl;
    std::cout << " set [tag]                               =  Switches the current instance." << std::endl;

    std::cout << std::endl;

    std::cout << "Agent source commands" << std::endl;
    std::cout << " add [name] <epoch>              =  adds a new metric of [name] to the system." << std::endl;
    std::cout << " remove  [name]                  =  removes a metric of [name]." << std::endl;
    std::cout << " update  [name] [value] <epoch>  =  Updates a metric." << std::endl;
    std::cout << " updates [name] [value] <epoch>  =  Updates a metric with a string value." << std::endl;
    std::cout << " send                            =  Gives time to the API instance to send updates." << std::endl;

    std::cout << std::endl;
}

void sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM )
        std::cout << "Interrupt caught. Use 'quit' to exit." << std::endl;
    return;
}

void  sleeps ( int secs )
{
#   ifdef WIN32
    Sleep(secs);
#   else
    ::sleep(secs);
#   endif
}


int sendUpdates ( TnmsAPI * api, time_t & now )
{
    int     retval = 0;
    int     errcnt = 0;
    size_t  qsize  = 0;

    if ( api == NULL )
        return -1;

    if ( ! connection )
        std::cout << "Connecting.";
    do {

#       ifndef WIN32
        struct tms  clocktms;
        clock_t     clk1, clk2;
        clk1 = times(&clocktms);
#       endif

        retval = api->send(now);

#   ifndef WIN32
        clk2 = times(&clocktms);

        if ( retval == 0 )
            std::cout << "send() took " << (clk2 - clk1) << " tick(s)" << std::endl;
#   endif

        sleeps(1);
       
        if ( retval == 1 ) {
            std::cout << "Invalid configuration" << std::endl;
            break;
        }

        if ( retval > 0 )
            retval = api->send(now + 1);
        
        qsize  = api->flushsize();
        if ( qsize > 0 )
            std::cout << "Bytes queued: " << qsize << std::endl;
        
        if ( connection )
            sleeps(1);

        if ( retval > 0 ) {
            if ( ! connection ) {
                std::cout << ".";
                std::cout.flush();
            } else if ( retval == TNMSERR_CONN_DENIED ) {
                std::cout << "Not authorized." << std::endl;
                connection = false;
                break;
            }
            errcnt++;
        } else {
            if ( ! connection )
                std::cout << std::endl << "Connected.";
            connection = true;
        }
    } while ( (retval > 0 && errcnt < 8) );

    std::cout << std::endl;

    return retval;
}



int  runConsole ( std::istream & istrm, bool showprompt, bool echo = false )
{
    std::string  cmd, name, cfgfile, agentname, desc, timestr;
    std::string  prompt = "[tnms]: ";

    std::vector<std::string>  cmdlist;

    ApiMap              apimap;
    ApiMap::iterator    showI, aIter;
    
    uint64_t  val;
    time_t    ts, now;
    char      line[1024];

    size_t    len    = 1024;
    size_t    lnlen  = 0;
    bool      done   = false;


    if ( ! istrm ) 
        return -1;

    if ( showprompt ) {
        std::cout << prompt;
        LogFacility::OpenLogStream("stdout", &std::cout);
        LogFacility::SetBroadcast(true);
    }

    while ( ! done && istrm.getline(line, len) )
    {
        std::vector<std::string> cmdlist;

        if ( echo )
            std::cout << line << std::endl;

        lnlen = ::strnlen(line, 1024);
        if ( lnlen > 0 ) {
            if ( line[0] == '#' )
                continue;
        }

        StringUtils::split(line, ' ', std::back_inserter(cmdlist));

        if ( cmdlist.size() > 0 )
            cmd  = cmdlist[0];
        else
            cmd  = "";

        now  = ::time(NULL);

        // CREATE
        //
        if ( cmd.compare("create") == 0 )
        {
            // create instance
            if ( cmdlist.size() < 3 ) {
                std:: cout << "Error: invalid syntax" << std::endl;
                continue;
            }

            name      = cmdlist[1];
            agentname = cmdlist[2];

            if ( apimap.find(name) != apimap.end() ) {
                std::cout << "Instance already exists for '" << name << "'" << std::endl;
                continue;
            }
            TnmsAPI * api = new TnmsAPI(agentname);

            if ( cmdlist.size() == 4 ) {
                cfgfile = cmdlist[3];
                api->set_config(cfgfile);
            }
            
            ApiMapInsert  insertR = apimap.insert(ApiMap::value_type(name, api));
            if ( ! insertR.second ) {
                std::cout << "Error: Insert failed for '" << name << "'" << std::endl;
                continue;
            }
            showI = insertR.first;
            prompt = "[tnms : " + showI->first + "]: ";
        }
        //
        // DESTROY
        //
        else if ( cmd.compare("destroy") == 0 )
        {
            // destroy instance
            if ( cmdlist.size() != 2 ) {
                std:: cout << "Error: invalid syntax" << std::endl;
                continue;
            }

            name = cmdlist[1];

            ApiMap::iterator  aIter = apimap.find(name);

            if ( aIter == apimap.end() ) {
                std::cout << "Error: '" << name << "' not found." << std::endl;
                continue;
            }
            if ( aIter == showI ) {
                showI = apimap.end();
                prompt = "[tnms : n/a]: ";
            }
            delete aIter->second;
            apimap.erase(aIter);
        }
        //
        // LIST
        //
        else if ( cmd.compare("list") == 0 )
        {
            for ( aIter = apimap.begin(); aIter != apimap.end(); ++aIter )
                std::cout << aIter->first << std::endl;
        } 
        //
        // SET
        //
        else if ( cmd.compare("set") == 0 )
        {
            if ( cmdlist.size() != 2 ) {
                std::cout << "Error: invalid syntax" << std::endl;
                continue;
            }

            name = cmdlist[1];

            aIter = apimap.find(name);
            if ( aIter == apimap.end() ) {
                std::cout << "Error: '" << name << "' not found." << std::endl;
                continue;
            }
            showI  = aIter;
            prompt = "[tnms : " + showI->first + "]: ";
        }
        //
        // ADD
        //
        else if ( cmd.compare("add") == 0 )
        {
            if ( cmdlist.size() < 2 ) {
                std::cout << "Error: invalid syntax" << std::endl;
                continue;
            }
            if ( showI == apimap.end() ) {
                std::cout << "No valid instance in this context" << std::endl;
                continue;
            }

            name   = cmdlist[1];
            if ( cmdlist.size() == 3 )
                ts = StringUtils::fromString<time_t>(cmdlist[2]);
            else
                ts = now;

            //  api->add
            if ( ! showI->second->add(name, ts, "") ) {
                std::cout << "Error: add failed for '" << name << "'" << std::endl;
                continue;
            }
        }
        //
        // UPDATE
        //
        else if ( cmd.compare("update") == 0 )
        {
            if ( cmdlist.size() < 3 ) {
                std::cout << "Error: invalid syntax" << std::endl;
                continue;
            }
            if ( showI == apimap.end() ) {
                std::cout << "No valid instance in this context" << std::endl;
                continue;
            }

            name = cmdlist[1];
            val  = StringUtils::fromString<uint64_t>(cmdlist[2]);

            if ( cmdlist.size() == 4 )
                ts = StringUtils::fromString<time_t>(cmdlist[3]);
            else
                ts = now;

            // api->update
            if ( ! showI->second->update(name, ts, val, TNMS_UINT64) ) {
                std::cout << "Error: update failed" << std::endl;
                continue;
            }
        }
        // 
        // UPDATE_S
        //
        else if ( cmd.compare("updateS") == 0 )
        {
            if ( cmdlist.size() < 3 ) {
                std::cout << "Error: invalid syntax" << std::endl;
                continue;
            }
            if ( showI == apimap.end() ) {
                std::cout << "No valid instance in this context" << std::endl;
                continue;
            }

            name = cmdlist[1];
            desc = cmdlist[2];

            if ( cmdlist.size() == 4 )
                ts = StringUtils::fromString<time_t>(cmdlist[3]);
            else
                ts = ::time(NULL);

            if ( ! showI->second->update(name, ts, desc) ) {
                std::cout << "Error: update failed" << std::endl;
                continue;
            }
        }
        // 
        // REMOVE
        //
        else if ( cmd.compare("remove") == 0 )
        {
            if ( cmdlist.size() < 2 ) {
                std::cout << "Error: invalid syntax" << std::endl;
                continue;
            }
            if ( showI == apimap.end() ) {
                std::cout << "No valid instance in this context" << std::endl;
                continue;
            }

            name = cmdlist[1];

            // api->remove
            if ( ! showI->second->remove(name) ) {
                std::cout << "Error: remove failed" << std::endl;
                continue;
            }
        }
        // 
        // SEND
        //
        else if ( cmd.compare("send") == 0 )
        {
            // send updates
            if ( showI == apimap.end() ) {
                std::cout << "No valid instance in this context" << std::endl;
                continue;
            }

            int ret = 0;
            if ( (ret = sendUpdates(showI->second, now)) != 0 ) {
                std::cout << "no connection" << std::endl;
                //continue;
            }
        }
        else if ( (cmd.compare("help") == 0) || (cmd.compare("?") == 0) )
        {
            displayHelp();
        }
        else if ( cmd.compare("debug") == 0 )
        {
           if ( showI == apimap.end() ) {
                std::cout << "No valid instance in this context" << std::endl;
                continue;
            }
            
            // dump tree
        } 
        else if ( cmd.compare("quit") == 0 ) 
        {
            done = true;
            break;
        }
        else 
        {
            if ( lnlen )
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

    signal(SIGINT,  &sigHandler);
    signal(SIGTERM, &sigHandler);

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

