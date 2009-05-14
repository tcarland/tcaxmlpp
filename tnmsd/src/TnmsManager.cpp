#define _TNMSD_TNMSMANAGER_CPP_

#include <time.h>

#include "TnmsManager.h"

#include "LogFacility.h"


namespace tnmsd {


TnmsManager::TnmsManager ( const std::string & configfile )
    : _evmgr(new tcanetpp::EventManager()),
      _tree(new TnmsTree()),
      _agtsvr(NULL),
      _clnsvr(NULL),
      _auth(NULL),
      _agtsvrid(0),
      _clnsvrid(0),
      _agentHandler(new AgentIOHandler(_tree)),
      _clientHandler(new ClientIOHandler(_tree)),
      _lastTouched(0),
      _logRotate(0),
      _startDelay(DEFAULT_STARTUP_DELAY),
      _startAt(0),
      _reportAt(0),
      _holddown(DEFAULT_HOLDDOWN_INTERVAL),
      _today(0),
      _configfile(configfile),
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

    delete _agentHandler;
    delete _clientHandler;
    delete _tree;
    delete _evmgr;
}


void
TnmsManager::run()
{
    time_t  now = ::time(NULL);

    LogFacility::SetLogTime(now);

    if ( _debug ) {
        LogFacility::OpenLogStream("stdout", &std::cout);
        LogFacility::LogMessage("TnmsManager::run()");
        LogFacility::SetBroadcast(true);
    }

    _logRotate = now + LOG_ROTATE_INTERVAL;
    _reportAt  = now + _holddown;
    
    _evmgr->addTimerEvent( (EventTimerHandler*) this, 5, 0);

    if ( ! this->parseConfig(_configfile, now) ) {
        // error parsing config
        if ( _debug )
            LogFacility::LogMessage("Error parsing config");
        return;
    }

    //  Enter Main Loop
    _evmgr->eventLoop();

    LogFacility::CloseLogFacility();

    return;
}


void
TnmsManager::timeout ( const EventTimer * timer )
{
    const time_t & now = timer->abstime.tv_sec;

    LogFacility::SetLogTime(now);

    // if startup_delay is expired; then add IOHandler for clients
    if ( _startDelay > 0 ) {
        if ( _startAt == 0 )
            _startAt = now + _startDelay;

        if ( _startAt <= now && _clnsvr ) {
            _clnsvrid = _evmgr->addIOEvent(_clientHandler, _clnsvr->getDescriptor(), _clnsvr, true);
            _startDelay = 0;
        }
    }

    // re-parse config signal
    if ( _hup ) { 
        if ( ! this->parseConfig(_configfile, now) )
            LogFacility::LogMessage("Config error");
        _hup = false;
    }

    if ( _usr ) {
        // special function
    }

    //check for logrotate
    if ( ! _tconfig.syslog && _logRotate <= now )
        this->logRotate(_tconfig.logfile, now);

    // give time to our handlers
    //_auth->timeout(timer);
    _agentHandler->timeout(timer);
    _clientHandler->timeout(timer);

    // internal metrics
    if ( _reportAt <= now ) {
        //send internal updates
        LogFacility::LogMessage("TnmsManager::run()"); // <<add some stats
        _reportAt = now + _holddown;
    }

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

    if ( _debug )
        LogFacility::LogMessage("TnmsManager::createClients()");

    for ( cIter = clist.begin(); cIter != clist.end(); ++cIter ) {
        client = new TnmsClient(_tree);

        // set client attributes
        //client->setReconnectInterval(cIter->reconnect_interval);
        _agentHandler->addMirrorConnection(client);

        if ( client->openConnection(cIter->hostname, cIter->port)  < 0 )
            continue;

        _evmgr->addIOEvent(_agentHandler, client->getDescriptor(), client);

        // client->login();
    }

    return;
}


bool
TnmsManager::parseConfig ( const std::string & cfg, const time_t & now )
{
    TnmsConfigHandler  cfgmgr(cfg, TNMSD_CONFIG_ROOT);
    std::string        prefix = TNMSD_CONFIG_ROOT;

    if ( ! cfgmgr.parse() ) {
        if ( LogFacility::IsOpen() ) {
            LogFacility::LogMessage("TnmsManager::parseConfig() error: " 
                    + cfgmgr.getErrorStr());
        } else {
            fprintf(stderr, "TnmsManager::parseConfig() error: %s\n",
                    cfgmgr.getErrorStr().c_str());
        }
        return false;
    }
    TnmsConfig & config = cfgmgr.config;

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

    TnmsServerConfig & nsvrcfg = cfgmgr.config.serverConfig;
    TnmsServerConfig & svrcfg  = _tconfig.serverConfig;

    // agent server startup
    if ( nsvrcfg.agent_port != svrcfg.agent_port 
            && nsvrcfg.agent_port > 0 )
    {
        LogFacility::Message  logmsg;
        logmsg << "TnmsManager::parseConfig(): Agent server listening on port " 
               << nsvrcfg.agent_port;
        LogFacility::LogMessage(logmsg.str());

        if ( _agtsvr ) {
            _agtsvr->close();
            _evmgr->removeEvent(_agtsvrid);
        }

        _agtsvr = new Socket(0, nsvrcfg.agent_port, SOCKET_SERVER, SOCKET_TCP);

        if ( ! _agtsvr->init(false) ) {
            LogFacility::LogMessage("TnmsManager::parseConfig: Error creating agent server socket" 
                    + _agtsvr->errorStr());
            return false;
        }

        _agtsvrid = _evmgr->addIOEvent(_agentHandler, _agtsvr->getFD(), _agtsvr, true);
    } 

    // client server startup
    if ( nsvrcfg.client_port != svrcfg.client_port 
            && nsvrcfg.client_port > 0 )
    {
        LogFacility::Message  logmsg;
        logmsg << "TnmsManager::parseConfig(): Client Server listening on port " 
               << nsvrcfg.client_port;
        LogFacility::LogMessage(logmsg.str());

        if ( _clnsvr ) {
            _clnsvr->close();
            _evmgr->removeEvent(_clnsvrid);
        }

        _clnsvr = new Socket(0, nsvrcfg.client_port, SOCKET_SERVER, SOCKET_TCP);

        if ( ! _clnsvr->init(false) ) {
            LogFacility::LogMessage("TnmsManager::parseConfig: Error creating client server socket" 
                    + _clnsvr->errorStr());
            return false;
        }

        if ( _startDelay == 0 )
            _clnsvrid = _evmgr->addIOEvent(_clientHandler, _clnsvr->getFD(), _clnsvr, true);
    }

    // init auth client 

    // assign the new config
    _tconfig = cfgmgr.config;

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
        LogFacility::OpenLogFile(LogFacility::GetLogPrefix(), logfile, true);
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
    return this->_errstr;
}


} // namespace

// _TNMSD_TNMSMANAGER_CPP_

