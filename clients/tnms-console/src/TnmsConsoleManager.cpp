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

    LogFacility::OpenLogStream("stdout", &std::cout);
    LogFacility::LogMessage("TnmsConsoleManager::run()");
    LogFacility::SetBroadcast(true);
    LogFacility::SetLogPrefix("[tnms] :");

    _console->start();

    _evmgr->addTimerEvent( (EventTimerHandler*) this, 5, 0);
    _evmgr->eventLoop();

    LogFacility::CloseLogFacility();

    return;
}


void
TnmsConsoleManager::timeout ( const EventTimer * timer )
{
    int   rd, sd;

    const time_t & now = timer->abstime.tv_sec;
    LogFacility::SetLogTime(now);
    ClientMap::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) {
        TnmsClient * client = (TnmsClient*) cIter->second;
        LogFacility::Message  logmsg;

        logmsg << "TnmsConsoleManager::timeout (" << client->getHostStr() << ")";

        if ( ! client->isConnected() || client->isConnecting() ) {
            int c = 0;

            if ( (c = client->connect()) < 0 ) {
                logmsg << " client disconnected.";
                LogFacility::LogMessage(logmsg);
                continue;
            } else if ( c >= 0 ) {
                timer->evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);
                continue;
            }

            if ( c > 0 ) {
                logmsg << " client connected.";
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
            logmsg << " error in receive() ";
            LogFacility::LogMessage(logmsg);
            client->close();
            continue;
        } else {
            // rd = ?
            ;
        }

        if ( (sd = client->send()) < 0 ) {
            logmsg << " error in send() ";
            LogFacility::LogMessage(logmsg);
            client->close();
            continue;
        } else {
            // sd is our count
            ;
        }

    }

    // check for client commands
    std::string  cmd;
    if ( _console->isRunning() ) {
        if ( _console->getClientCommand(cmd) )
            this->runClientCommand(cmd);
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

    _clients[name] = client;

    _evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);

    return true;
}

bool
TnmsConsoleManager::closeClient ( const std::string & name )
{
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

    return true;
}


void
TnmsConsoleManager::runClientCommand ( const std::string & cmd )
{

}


void
TnmsConsoleManager::setAlarm()
{
    _evmgr->setAlarm();
}


//TnmsConsoleManager::TnmsConsoleManager
//TnmsConsoleManager::TnmsConsoleManager



}  // namespace


// _TNMSCONSOLE_MANAGER_CPP_

