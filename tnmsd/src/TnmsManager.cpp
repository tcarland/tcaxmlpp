#define _TNMSD_TNMSMANAGER_CPP_

#include <time.h>


#include "TnmsManager.h"
#include "TnmsClient.h"

#include "LogFacility.h"
using namespace tcanetpp;



namespace tnmsd {


TnmsManager::TnmsManager()
    : _evmgr(new EventManager()),
      _tree(new TnmsTree()),
      _server(NULL),
      _client(NULL),
      _auth(NULL),
      _svrid(0),
      _clid(0),
      _lastTouched(0),
      _logRotate(0),
      _startDelay(DEFAULT_STARTUP_DELAY),
      _startat(0),
      _today(0),
      _hup(false),
      _usr(false),
      _debug(false)
{}

TnmsManager::~TnmsManager()
{
    ClientMap::iterator cIter;

    for ( cIter = _clientMap.begin(); cIter != _clientMap.end(); ++cIter ) {
        TnmsClient * client = (TnmsClient*) cIter->second;

        if ( client ) {
            if ( client->isConnected() )
                client->close();
            delete client;
        }
    }
    _clientMap.clear();

    if ( _auth )
        delete _auth;

    if ( _server )
        delete _server;

    if ( _client )
        delete _client;
    
    delete _evmgr;
    delete _tree;
}


void
TnmsManager::timeout ( const EventTimer * timer )
{
    const time_t & now = timer->abstime.tv_sec;

    LogFacility::SetLogTime(now);

    // if startup_delay is expired; then add IOHandler for clients
    if ( _startupDelay > 0 ) {
        if ( _startup == 0 )
            _startup = now + _startupDelay;

        if ( _startup < now ) {
            _clid = _evmgr->addIOEvent(&_clientHandler, _client->getFD(), _client, true);
            _startupDelay = 0;
        }
    }

    if ( _hup ) {
        // parse config
        this->parseConfig(now);
        //if ( ! this->parseConfig(now) )
            //log error
        _hup = false;
    }

    if ( _usr ) {
        // special function
    }

    //check for logrotate
    if ( ! _tconfig.syslog && _logRotate <= now )
        this->logRotate(logfile, now);

    // do timeouts
    //_auth->timeout(timer);
    _agentHandler.timeout(timer);
    _clientHandler.timeout(timer);

    // update our metrics
    //if ( lastUpdate <= now )
    //    this->update(now);

    return;
}


void
TnmsManager::run()
{
    time_t  now = ::time(NULL);

    LogFacility::SetLogTime(now);

    if ( _debug )
        LogFacility::InitLogFacility("stdout", &std::cout);

    _logRotate = now + LOG_ROTATE_INTERVAL;
    //updates = now + HOLDDOWN_INTERVAL;
    
    _evmgr->addTimerEvent( (EventTimerHandler*) this, 5, 0);

    if ( ! this->parseConfig(now) ) {
        // error parsing config
        return;
    }

    //  Enter Main Loop
    _evmgr->eventLoop();

    LogFacility::Close();

    return;
}


void
TnmsManager::verifyClients ( const time_t & now )
{
}


void
TnmsManager::createClients()
{
    TnmsClient * client = NULL;

    ClientList  & clist  = _tconfig.clients;
    ClientList::iterator  cIter;

    for ( cIter = clist.begin(); cIter != clist.end(); ++cIter ) {
        client = new TnmsClient(_tree);

        // set client attributes
        //client->setReconnectInterval(cIter->reconnect_interval);
        _agentHandler.addMirrorConnection(client);

        if ( client->OpenConnection(cIter->hostaddr, cIter->port)  < 0 )
            continue;

        _evmgr->addIOEvent(&_agentHandler, client->getFD(), client);

        // client->login();
    }

    return;
}


bool
TnmsManager::parseConfig ( const std::string & cfg, const time_t & now )
{
    TnmsConfig  config(cfg, TNMSD_ROOT);
    std::string prefix = TNMSD_ROOT;

    if ( ! config.parse() ) {
        if ( LogFacility::isOpen() ) {
            LogFacility::LogMessage("TnmsManager::parseConfig() error: " 
                    + config.getErrorStr());
        } else {
            fprintf(stderr, "TnmsManager::parseConfig() error: %s\n",
                    config.getErrorStr());
        }
        return false;
    }

    prefix.append(".").append(config.agent_name);

    if ( config.syslog ) {
        LogFacility::OpenSyslog(prefix, LOG_LOCAL3);
    } else {
        if ( _tconfig.syslog )
            LogFacility::CloseSyslog();
        LogFacility::SetLogPrefix(prefix);
        this->logRotate(config.logfile, now);
    }

    // (re)initialize our tree?


    TnmsServerConfig & nsvrcfg = config.serverConfig;
    TnmsServerConfig & svrcfg  = _tconfig.serverConfig;

    // agent server startup
    if ( nsvrcfg.agent_listenport != svrcfg.agent_listenport 
            && nsvrcfg.agent_listenport > 0 )
    {
        LogFacility::Message  logmsg;
        logmsg << "Agent server listen port set to " 
               << nsvrcfg.agent_listenport;
        LogFacility::LogMessage(logmsg.str());

        if ( _server ) {
            _server->close();
            _evmgr->removeEvent(_svrid);
        }

        _server = new Socket(0, nsvrcfg.agent_listenport, SOCKET_SERVER, TCP);

        if ( ! _server->init(false) ) {
            LogFacility::LogMessage("Error creating agent server socket" 
                    + _server->errorStr());
            return false;
        }

        _svrid = _evmgr->addIOEvent(&_agentHandler, _server->getFD(), 
                                     _server, true);
    } 

    // client server startup
    if ( nsvrcfg.client_listenport != svrcfg.client_listenport 
            && nsvrcfg.client_listenport > 0 )
    {
        LogFacility::Message  logmsg;
        logmsg << "Client server listen port set to " 
               << nsvrcfg.client_listenport;
        LogFacility::LogMessage(logmsg.str());

        if ( _client ) {
            _client->close();
            _evmgr->removeEvent(_clid);
        }

        _client = new Socket(0, nsvrcfg.client_listenport, SOCKET_SERVER, TCP);

        if ( ! _client->init(false) ) {
            LogFacility::LogMessage("Error creating agent server socket" 
                    + _server->errorStr());
            return false;
        }

        _clid = _evmgr->addIOEvent(&_clientHandler, _client->getFD(), _client, true);
    }

    // init auth client 

    // assign the new config
    _tconfig = config;

    // init mirror connections
    if ( _tconfig.clients.size() > 0 )
        this->createClients();

    return true;
}


void
TnmsManager::logRotate ( std::string logfile, const time_t & now )
{
    char        datestr[64];
    struct tm  *ltm   = ::localtime(&now);
    int         today = ltm->tm_yday;

    if ( logfile.empty() ) 
        return;

    if ( _today != today ) {
        ::strftime(datestr, 64, ".%Y%02m%02d", ltm);
        logfile.append(datestr);
        Logger::OpenLogFile(logfile, true);
        _today = today;
    }

    _logRotate = now + LOG_ROTATE_INTERVAL;

    return;
}


void
TnmsManager::setAlarm()
{
    this->_evmgr->setAlarm();
}


void
TnmsManager::setHUP()
{
    this->_hup = true;
}


void
TnmsManager::setUSR()
{
    this->_usr = true;
}


void
TnmsManager::setDebug ( bool d )
{
    this->_debug = d;
}


std::string
TnmsManager::getErrorStr()
{
    return _errstr;
}


} // namespace

// _TNMSD_TNMSMANAGER_CPP_

