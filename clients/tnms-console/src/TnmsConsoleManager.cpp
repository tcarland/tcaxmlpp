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

    LogFacility::OpenLogStream("console", "[tnms]:", &std::cout);
    //LogFacility::SetBroadcast(true);
    
    LogFacility::LogMessage("console", "TnmsConsoleManager::run() starting console...");

    _console->start();

    _evmgr->addTimerEvent( (EventTimerHandler*) this, 1, 0);
    _evmgr->eventLoop();

    LogFacility::LogMessage("console", "TnmsConsoleManager shutting down");
    LogFacility::RemoveLogStream("console");
    LogFacility::CloseLogFacility();

    return;
}


void
TnmsConsoleManager::timeout ( const EventTimer * timer )
{
    const time_t & now = timer->abstime.tv_sec;
    LogFacility::SetLogTime(now);

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

    _clientHandler->insert(name, client);
    _evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);

    LogFacility::LogMessage("console", "Created new client: " + client->getHostStr());

    return true;
}

bool
TnmsConsoleManager::removeClient ( const std::string & name )
{
    _clientHandler->erase(name);
    return true;
}


void
TnmsConsoleManager::runClientCommand ( const CommandList & cmdlist )
{
    TnmsClient * client = NULL;
    std::string  cmd, tag, name, val;

    if ( cmdlist.size() < 2 || cmdlist[0].compare("client") != 0 ) {
        LogFacility::LogToStream("console", "Error in client command");
        return;
    }

    cmd = cmdlist[1];

    // 
    //  New Client
    if ( cmd.compare("new") == 0 ) 
    {
        if ( cmdlist.size() < 5 ) {
            LogFacility::LogToStream("console", "Error in client command");
            return;
        }

        tag  = cmdlist[2];
        name = cmdlist[3];
        val  = cmdlist[4];

        uint16_t port = StringUtils::fromString<uint16_t>(val);
        
        if ( ! this->createClient(tag, name, port) )
            LogFacility::LogMessage("console", "Failed to create new client");

    }
    // Delete Client
    else if ( cmd.compare("del") == 0 ) 
    {
        LogFacility::LogMessage("console", "client del");

        if ( cmdlist.size() < 3 ) {
            LogFacility::LogToStream("console", "Error in client command");
            return;
        }

        tag = cmdlist[2];

        this->removeClient(tag);
    } 
    else if ( cmd.compare("list") == 0 ) 
    {
        _clientHandler->listClients();
    }
    else if ( cmd.compare("subscribe") == 0 ) 
    {
        if ( cmdlist.size() < 4 ) {
            LogFacility::LogToStream("console", "Syntax error in client subscribe");
            return;
        }
        tag    = cmdlist[2];
        name   = cmdlist[3];
        LogFacility::LogMessage("console", "client subscribe " + name);
        client = _clientHandler->find(tag);
        if ( client )
            client->subscribe(name);
    }
    else if ( cmd.compare("unsubscribe") == 0 ) 
    {
        if ( cmdlist.size() < 4 ) {
            LogFacility::LogToStream("console", "Syntax error in client unsubscribe");
            return;
        }
        tag    = cmdlist[2];
        name   = cmdlist[3];
        LogFacility::LogMessage("console", "client unsubscribe");
        client = _clientHandler->find(tag);
        if ( client )
            client->unsubscribe(name);
    }
    else if ( cmd.compare("browse") == 0 ) 
    {
         if ( cmdlist.size() < 3 )
             return;
         
         name = cmdlist[2];
         _tree->debugDump(name);
    }
    else if ( cmd.compare("dump") == 0 ) 
    {
        if ( cmdlist.size() < 3 )
             return;
         
        name = cmdlist[2];

        _tree->debugDump(name);
    }
    else if ( cmd.compare("show") == 0 ) 
    {
        if ( cmdlist.size() < 3 )
            return;

        TnmsMetric metric;
        name = cmdlist[2];
        
        _tree->request(name, metric);

         LogFacility::LogMessage(" Node: " + metric.getElementName());
    }
    else
    {
        LogFacility::LogMessage("console", "client command not recognized: " + cmd);
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

