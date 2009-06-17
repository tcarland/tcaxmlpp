#define _TNMSCONSOLE_MANAGER_CPP_

#include "TnmsConsoleManager.h"


#include "LogFacility.h"


namespace tnmsconsole {



TnmsConsoleManager::TnmsConsoleManager ( std::istream & istrm, bool interactive, bool echo )
    : _evmgr(new EventManager()),
      _tree(new TnmsTree()),
      _console(new ConsoleThread(istrm, interactive, echo)),
      _clientHandler(new ClientIOHandler(_tree)),
      _debug(false)
{}


TnmsConsoleManager::~TnmsConsoleManager()
{
    /*
    ClientMap::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) 
    {
        TnmsClient * client = (TnmsClient*) cIter->second;

        if ( client ) {
            if ( client->isConnected() )
                client->close();
            delete cIter->second;
        }
    }
    _clients.clear();
    */
    delete _evmgr;
    delete _tree;

    if ( _console ) {
        if ( _console->isRunning() )
            _console->stop();
        delete _console;
    }
}


void
TnmsConsoleManager::run()
{
    LogFacility::InitThreaded();

    LogFacility::OpenLogStream("console", &std::cout);
    LogFacility::SetLogPrefix("[tnms]:");
    //LogFacility::SetBroadcast(true);
    
    LogFacility::LogMessage("TnmsConsoleManager::run() starting console...");

    _console->start();

    _evmgr->addTimerEvent( (EventTimerHandler*) this, 5, 0);
    _evmgr->eventLoop();

    LogFacility::CloseLogFacility();

    return;
}


void
TnmsConsoleManager::timeout ( const EventTimer * timer )
{
    //int   rd, sd;

    const time_t & now = timer->abstime.tv_sec;
    LogFacility::SetLogTime(now);
    /*
    ClientMap::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) {
        TnmsClient * client = (TnmsClient*) cIter->second;
        LogFacility::Message  logmsg;

        logmsg << "TnmsConsoleManager::timeout (" << client->getHostStr() << ")";

        if ( ! client->isConnected() || client->isConnecting() ) {
            int c = 0;

            if ( (c = client->connect()) < 0 ) {
                logmsg << " client disconnected." << std::endl;
                LogFacility::LogMessage(logmsg);
                continue;
            } else if ( c >= 0 ) {
                logmsg << " client (re)connecting..." << std::endl;
                LogFacility::LogMessage(logmsg);
                timer->evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);
                continue;
            }

            if ( c > 0 ) {
                logmsg << " client connected." << std::endl;
                LogFacility::LogMessage(logmsg);
            }
        } else {
            if ( client->isAuthorized() && ! client->isSubscribed() ) {
                // suball?
                ;
            } else if ( ! client->isAuthorized() ) {
                client->login(_user, _pw);
            }
        }

        if ( (rd = client->receive()) < 0 ) {
            logmsg << " error in receive() " << std::endl;
            LogFacility::LogMessage(logmsg);
            client->close();
            continue;
        } else {
            // rd = ?
            ;
        }

        if ( (sd = client->send()) < 0 ) {
            logmsg << " error in send() " << std::endl;
            LogFacility::LogMessage(logmsg);
            client->close();
            continue;
        } else {
            // sd is our count
            ;
        }

    }
    */
    _clientHandler->timeout(timer);

    // check for client commands
    CommandList   cmdlist;
    if ( _console->isRunning() ) {
        while ( _console->getClientCommand(cmdlist) )
            this->runClientCommand(cmdlist);
    } else {
        this->setAlarm();
    }

    return;
}


bool
TnmsConsoleManager::createClient ( const std::string & name, const std::string & host, uint16_t port )
{
    TnmsClient * client = new TnmsClient(this->_tree);

    if ( client->openConnection(host, port) < 0 )
        return false;

    //_clients[name] = client;
    _clientHandler->insert(name, client);

    _evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);

    return true;
}

bool
TnmsConsoleManager::removeClient ( const std::string & name )
{
    _clientHandler->erase(name);
    /*
    ClientMap::iterator  cIter;

    cIter = _clients.find(name);

    if ( cIter == _clients.end() )
        return false;

    TnmsClient * client = cIter->second;

    if ( client ) {
        client->close();
        delete client;
    }
    _clients.erase(cIter);
    */
    return true;
}


void
TnmsConsoleManager::runClientCommand ( const CommandList & cmdlist )
{
    std::string  cmd, tag, name, val;

    if ( cmdlist.size() < 2 || cmdlist[0].compare("client") != 0 ) {
        LogFacility::LogToStream("console", "Error in client command");
        return;
    }

    cmd = cmdlist[1];

    if ( cmd.compare("new") == 0 ) {
        LogFacility::LogMessage("client new");
        
        if ( cmdlist.size() < 5 ) {
            LogFacility::LogToStream("console", "Error in client command");
            return;
        }

        tag  = cmdlist[2];
        name = cmdlist[3];
        val  = cmdlist[4];

        uint16_t port = StringUtils::fromString<uint16_t>(val);
        
        if ( ! this->createClient(tag, name, port) )
            LogFacility::LogMessage("Failed to create new client");

    } else if ( cmd.compare("del") == 0 ) {
        LogFacility::LogMessage("client del");

        if ( cmdlist.size() < 3 ) {
            LogFacility::LogToStream("console", "Error in client command");
            return;
        }

        tag = cmdlist[2];

        this->removeClient(tag);
    } else if ( cmd.compare("list") == 0 ) {
        LogFacility::LogMessage("client list");

        this->listClients();
    } else if ( cmd.compare("subscribe") == 0 ) {
        LogFacility::LogMessage("client subscribe");
    } else {
        LogFacility::LogMessage("client command not recognized: " + cmd);
    }

    return;
}

void
TnmsConsoleManager::listClients()
{
    ClientMap::iterator  cIter;

    LogFacility::LogMessage(" Client List ");
    
    for ( cIter = _clientHandler->begin(); cIter != _clientHandler->end(); ++cIter ) {
        LogFacility::Message  msg;

        msg << "    "  << cIter->first << "    =    " << cIter->second->getHostStr() << std::endl;
        LogFacility::LogMessage(msg);
    }

    return;
}

void
TnmsConsoleManager::setAlarm()
{
    _evmgr->setAlarm();
}


}  // namespace


// _TNMSCONSOLE_MANAGER_CPP_

