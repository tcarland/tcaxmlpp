#define _TNMSCONSOLE_CONSOLETHREAD_CPP_

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
}

#include <fstream>
#include <vector>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/times.h>
#endif


#include "ConsoleThread.h"
#include "LogFacility.h"


namespace tnmsconsole {


ConsoleThread::ConsoleThread ( std::istream & istrm, bool showprompt, bool echo )
    : _istrm(istrm),
      _prompt(showprompt),
      _echo(echo)
{}

ConsoleThread::~ConsoleThread() {}


void
ConsoleThread::run()
{
    std::string  cmd, name, cfgfile, agentname, desc, timestr;
    std::string  prompt = "[tnms]";

    CommandList         cmdlist;
    ApiMap              apimap;
    ApiMap::iterator    showI, aIter;
    
    uint64_t  val;
    time_t    ts, now;
    char      line[1024];
    size_t    len    = 1024;
    size_t    lnlen  = 0;

    if ( ! this->_istrm ) 
        return;

    this->sleeps(3);

    if ( _prompt ) {
        LogFacility::Message logmsg;
        logmsg << " ";
        LogFacility::SetLogPrefix(prompt);
        LogFacility::LogToStream("stdout", logmsg.str(), false);
        prompt.append(": ");
    }

    showI = apimap.end();

    while ( ! this->_Alarm && _istrm.getline(line, len) )
    {
        LogFacility::Message  msg;
        std::vector<std::string> cmdlist;

        if ( _echo )
            LogFacility::LogToStream("stdout", line);

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
        
        now = LogFacility::GetLogTime();

        // CREATE
        //
        if ( cmd.compare("create") == 0 )
        {
            // create instance
            if ( cmdlist.size() < 3 ) {
                msg << "Error: invalid syntax " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            name      = cmdlist[1];
            agentname = cmdlist[2];

            if ( apimap.find(name) != apimap.end() ) {
                msg << "Instance already exists for '" << name << "' " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
            TnmsAPI * api = new TnmsAPI(agentname);
            api->set_debug(true);

            if ( cmdlist.size() == 4 ) {
                cfgfile = cmdlist[3];
                api->set_config(cfgfile);
            }
            
            ApiMapInsert  insertR = apimap.insert(ApiMap::value_type(name, api));
            if ( ! insertR.second ) {
                msg << "Error: Insert failed for '" << name << "' " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
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
                std:: cout << "Error: invalid syntax " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            name = cmdlist[1];

            ApiMap::iterator  aIter = apimap.find(name);

            if ( aIter == apimap.end() ) {
                msg << "Error: '" << name << "' not found. " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
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
                msg << aIter->first << std::endl;
        } 
        //
        // SET
        //
        else if ( cmd.compare("set") == 0 )
        {
            if ( cmdlist.size() != 2 ) {
                msg << "Error: invalid syntax " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            name = cmdlist[1];

            aIter = apimap.find(name);
            if ( aIter == apimap.end() ) {
                msg << "Error: '" << name << "' not found. " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
            showI  = aIter;
            prompt = "[tnms : " + showI->first + "]";
            LogFacility::SetLogPrefix(prompt);
            prompt.append(": ");
        }
        //
        // ADD
        //
        else if ( cmd.compare("add") == 0 )
        {
            if ( cmdlist.size() < 2 ) {
                msg << "Error: invalid syntax " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
            if ( showI == apimap.end() ) {
                msg << "No valid instance in this context. " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            name   = cmdlist[1];
            if ( cmdlist.size() == 3 )
                ts = StringUtils::fromString<time_t>(cmdlist[2]);
            else
                ts = now;

            //  api->add
            if ( ! showI->second->add(name, ts) ) {
                msg << "Error: add failed for '" << name << "'" << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
        }
        //
        // UPDATE
        //
        else if ( cmd.compare("update") == 0 )
        {
            if ( cmdlist.size() < 3 ) {
                msg << "Error: invalid syntax " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
            if ( showI == apimap.end() ) {
                msg << "No valid instance in this context " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            name = cmdlist[1];
            val  = StringUtils::fromString<uint64_t>(cmdlist[2]);

            if ( cmdlist.size() == 4 )
                ts = StringUtils::fromString<time_t>(cmdlist[3]);
            else
                ts = now;

            msg << "Update: '" << name << "' val: " << val << " tstamp: " << ts << std::endl;

            // api->update
            if ( ! showI->second->update(name, ts, val, TNMS_UINT64) ) {
                msg << "Error: update failed" << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
        }
        // 
        // UPDATE_S
        //
        else if ( cmd.compare("updateS") == 0 )
        {
            if ( cmdlist.size() < 3 ) {
                msg << "Error: invalid syntax " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
            if ( showI == apimap.end() ) {
                msg << "No valid instance in this context " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            name = cmdlist[1];
            desc = cmdlist[2];

            if ( cmdlist.size() == 4 )
                ts = StringUtils::fromString<time_t>(cmdlist[3]);
            else
                ts = ::time(NULL);

            if ( ! showI->second->update(name, ts, desc) ) {
                msg << "Error: update failed " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
        }
        // 
        // REMOVE
        //
        else if ( cmd.compare("remove") == 0 )
        {
            if ( cmdlist.size() < 2 ) {
                msg << "Error: invalid syntax " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
            if ( showI == apimap.end() ) {
                msg << "No valid instance in this context " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            name = cmdlist[1];

            // api->remove
            if ( ! showI->second->remove(name) ) {
                msg << "Error: remove failed " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
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
                msg << "No valid instance in this context " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }

            int ret = 0;
            if ( (ret = this->sendAPIUpdates(showI->second, now)) != 0 ) {
                msg << "no connection " << prompt;
                //continue;
            }
            prompt = "[tnms : " + showI->first + "]";
            LogFacility::SetLogPrefix(prompt);
            prompt.append(": ");
        }
        else if ( (cmd.compare("help") == 0) || (cmd.compare("?") == 0) )
        {
            ConsoleThread::DisplayHelp();
        }
        else if ( cmd.compare("debug") == 0 )
        {
           if ( showI == apimap.end() ) {
                msg << "No valid instance in this context " << prompt;
                LogFacility::LogToStream("stdout", msg.str(), false);
                continue;
            }
            
            // dump tree
        } 
        else if ( cmd.compare("quit") == 0 ) 
        {
            this->setAlarm();
            break;
        }
        else if ( cmd.compare("client") == 0 )
        {
            this->addClient
        }
        else 
        {
            if ( lnlen )
                msg << "Unrecognized command: '" << cmd << "'" << std::endl;
        }

        if ( ! this->_istrm.good() )
        {
            if ( _prompt )
                _istrm.clear();
            else
                this->setAlarm();
        }

        if ( _prompt )
            msg << " ";
        
        LogFacility::LogToStream("stdout", msg.str(), false);
    }

    return;
}

bool
ConsoleThread::addClientCommand ( CommandList & cmdlist )
{
    if ( this->_cmdQueue.push(cmdlist) )
        return true;
    return false;
}

bool
ConsoleThread::getClientCommand ( CommandList & cmdlist )
{
    if ( this->_cmdQueue.pop(cmdlist) )
        return true;
    return false;
}


int
ConsoleThread::sendAPIUpdates ( TnmsAPI * api, const time_t & now )
{
    int     retval = 0;
    int     errcnt = 0;
    size_t  qsize  = 0;
    bool    conn   = false;

    if ( api == NULL )
        return -1;

    //if ( ! connection )
        //msg << "Connecting.";
        //

    LogFacility::Message  logmsg;
    
    do {
#       ifndef WIN32
        struct tms  clocktms;
        clock_t     clk1, clk2;
        clk1 = times(&clocktms);
#       endif

        retval = api->send(now);

#       ifndef WIN32
        clk2 = times(&clocktms);
        if ( retval == 0 )
            logmsg << "send() took " << (clk2 - clk1) << " tick(s)" << std::endl;
#       endif

        this->sleeps(1);
       
        if ( retval == 1 ) {
            logmsg << "Invalid configuration" << std::endl;
            break;
        } else if ( retval > 0 ) {
            retval = api->send(now + 1);
        } else 
            conn = true;
        
        qsize  = api->flushsize();
        if ( qsize > 0 )
            logmsg << "Bytes queued: " << qsize << std::endl;
        
        if ( retval > 0 ) {
            if ( ! conn ) {
                logmsg << ".";
            } else if ( retval == TNMSERR_CONN_DENIED ) {
                logmsg << "Not authorized." << std::endl;
                conn = false;
                break;
            }
            errcnt++;
        } else {
            if ( ! conn )
                logmsg << std::endl << "Connected.";
            conn = true;
        }
    } while ( (retval > 0 && errcnt < 8) );

    logmsg << std::endl;

    LogFacility::LogToStream("stdout", logmsg.str(), false);

    return retval;
}



void
ConsoleThread::DisplayHelp()
{
    LogFacility::Message  logmsg;

    logmsg << std::endl;

    logmsg << " tnms_console - a command-line agent process " << std::endl;
    logmsg << "Usage: tnms-console [-noprompt] [script file]" << std::endl;

    logmsg << std::endl;

    logmsg << "TnmsAPI Instance commands:" << std::endl;
    logmsg << " create [tag] [agent-name] [configfile]  =  Creates a new instance." << std::endl;
    logmsg << " destroy [tag]                           =  Destroys an instance." << std::endl;
    logmsg << " list                                    =  Lists available instances." << std::endl;
    logmsg << " set [tag]                               =  Switches the current instance." << std::endl;

    logmsg << std::endl;

    logmsg << "Agent source commands" << std::endl;
    logmsg << " add [name] <epoch>                  =  adds a new metric of 'name'." << std::endl;
    logmsg << " remove  [name]                      =  removes a metric of 'name'." << std::endl;
    logmsg << " update  [name] [value] <epoch>      =  Updates a metric." << std::endl;
    logmsg << " updates [name] [value] <epoch>      =  Updates a metric with a string value." << std::endl;
    logmsg << " send                                =  Give time to the API instance." << std::endl;

    logmsg << std::endl;

    logmsg << "Client commands" << std::endl;
    logmsg << " client new [tag] [server] [port]   = creates a new client connection " << std::endl;
    logmsg << " client del [tag]                   = removes a client connection  " << std::endl;
    logmsg << " client list                        = lists current clients  " << std::endl;
    logmsg << " client subscribe [tag] [name]      = subscribes client 'tag' to 'name'  " << std::endl;
    logmsg << " client unsubscribe [tag] [name]    = unsubscribes client 'tag' to 'name'  " << std::endl;
    logmsg << " client browse [name]               = list subtree by 'name' " << std::endl;
    logmsg << " client dump   [name]               = dump subtree by 'name' " << std::endl;
    logmsg << " client show   [name]               = display the current metric for 'name' " << std::endl;

    logmsg << std::endl;

    LogFacility::LogToStream("stdout", logmsg.str());

    return;
}


void
ConsoleThread::sleeps ( int secs )
{
#ifdef WIN32
    Sleep(secs);
#else
    ::sleep(secs);
#endif
}


} // namespace



