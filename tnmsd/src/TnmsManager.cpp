#define _TNMSD_TNMSMANAGER_CPP_

#include <time.h>

#include "TnmsManager.h"
#include "AgentIOHandler.h"
#include "ClientIOHandler.h"

#include "AuthClient.h"

#include "LogFacility.h"
#include "FileUtils.h"



namespace tnmsd {


std::string
TnmsManager::_Version = "v0.17";


TnmsManager::TnmsManager ( const std::string & configfile )
    : _evmgr(new tcanetpp::EventManager()),
      _tree(new TnmsTree()),
      _agent(NULL),
      _client(NULL),
      _auth(new AuthClient(this->_evmgr)),
      _agentId(0),
      _clientId(0),
      _reportId(0),
      _logId(0),
      _agentHandler(new AgentIOHandler(_tree, _auth)),
      _clientHandler(new ClientIOHandler(_tree, _auth)),
      _lastTouched(0),
      _reportDelay(DEFAULT_REPORT_INTERVAL),
      _startDelay(DEFAULT_STARTUP_DELAY),
      _startAt(0),
      _logCheck(LOG_CHECK_INTERVAL),
      _holddown(DEFAULT_HOLDDOWN_INTERVAL),
      _today(0),
      _configfile(configfile),
      _hup(false),
      _usr(false),
      _debug(false),
      _verbose(false)
{}


TnmsManager::~TnmsManager()
{
    this->destroyClients();
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

    if ( _verbose ) {
        LogFacility::OpenLogStream("stdout", "", &std::cout);
        LogFacility::LogMessage("TnmsManager::run()");
        LogFacility::SetBroadcast(true);
    }

    _evmgr->addTimerEvent(this, 5, 0);
    _reportId  = _evmgr->addTimerEvent(this, _reportDelay, 0);
    _logId     = _evmgr->addTimerEvent(this, _logCheck, 0);

    if ( ! this->parseConfig(_configfile, now) ) {
        if ( _debug )
            LogFacility::LogMessage("Error parsing config");
        return;
    }

    _evmgr->eventLoop(); // main loop

    if ( _debug )
        LogFacility::RemoveLogStream("stdout");

    LogFacility::CloseLogFacility();

    return;
}


void
TnmsManager::timeout ( const EventTimer * timer )
{
    const time_t & now = timer->abstime.tv_sec;

    LogFacility::SetLogTime(now);

    //check for logrotate
    if ( timer->evid == _logId ) {
        if ( ! _tconfig.syslog )
            this->logRotate(_tconfig.logfile, now);
        return;
    }

    // internal metrics
    if ( timer->evid == _reportId ) {
        LogFacility::LogMessage("TnmsManager::run()");
        _clientHandler->sendStats();
        _agentHandler->sendStats();
        return;
    }

    // if startup_delay is expired; then add IOHandler for clients
    if ( _startDelay > 0 ) {
        if ( _startAt == 0 )
            _startAt = now + _startDelay;

        if ( _startAt <= now && _client ) {
            _clientId   = _evmgr->addIOEvent(_clientHandler, _client->getDescriptor(), _client, true);
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
        _tree->debugDump();
        _usr = false;
    }

    // give time to our handlers
    _auth->timeout(timer);
    _agentHandler->timeout(timer);
    _clientHandler->timeout(timer);

    return;
}


void
TnmsManager::createClients()
{
    TnmsClient * client = NULL;
    evid_t       id     = 0;

    ClientConfigList  & clist  = _tconfig.clients;
    ClientConfigList::iterator  cIter;

    if ( _debug )
        LogFacility::LogMessage("TnmsManager::createClients()");

    for ( cIter = clist.begin(); cIter != clist.end(); ++cIter ) {
        TnmsClientConfig & cfg = *cIter;

        client = new TnmsClient(_tree);

        // set client attributes
        client->setCompression(_tconfig.compression);
        client->setReconnectTime(cfg.reconnect_interval);
        _clientHandler->addMirror(client);

        if ( client->openConnection(cIter->hostname, cIter->port)  < 0 )
            continue;

        id = _evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);

        std::string  login = TNMS_AGENT_ID;
        login.append(":").append(_tconfig.agent_name);
        client->setClientLogin(login, "");
        client->login();

        MirrorConnection  mirror(id, *cIter, client);
        _clients[cIter->connection_name] = mirror;
    }

    return;
}


void
TnmsManager::destroyClients ()
{
    ClientMap::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter )
    {
        TnmsClient * client = cIter->second.client;
        if ( client ) 
        {
            _clientHandler->eraseMirror(client);
            _evmgr->removeEvent(cIter->second.id);
            client->close();
            delete client;
        }
    }
    _clients.clear();
}


bool
TnmsManager::parseConfig ( const std::string & cfg, const time_t & now )
{
    if ( _lastTouched > 0 && _lastTouched == FileUtils::LastTouched(cfg) )
        return true;

    TnmsConfigHandler  cfgmgr(cfg, TNMSD_CONFIG_ROOT);
    std::string        prefix    = TNMSD_CONFIG_ROOT;

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

    if ( ! config.agent_name.empty() )
        prefix = config.agent_name;

    LogFacility::SetDefaultLogPrefix(prefix);
    _clientHandler->setPrefix(prefix);
    _agentHandler->setPrefix(prefix);

    if ( config.syslog ) {
        if ( _tconfig.syslog )
            LogFacility::CloseSyslog();
        LogFacility::OpenSyslog(prefix, LOG_LOCAL3);
    } else {
        if ( _tconfig.syslog )
            LogFacility::CloseSyslog();
        if ( config.logfile.compare(_tconfig.logfile) != 0 )
            _today = 0;
        this->logRotate(config.logfile, now);
    }

    this->setDebug(config.debug);
    LogFacility::SetDebug(config.debug);

    // set auth server
    if ( cfgmgr.haveAttribute("auth_bypass") ) {
        LogFacility::LogMessage("TnmsManager: WARNING: Enabling Auth Bypass");
        this->_auth->enableAuthBypass(true);
    } else {
        this->_auth->setAuthServer(config.auth_server, config.auth_port);
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

        if ( _agent ) {
            _agent->close();
            _evmgr->removeEvent(_agentId);
            _agentId = 0;
        }

        _agent = new Socket(0, nsvrcfg.agent_port, SOCKET_SERVER, SOCKET_TCP);

        if ( ! _agent->init(false) ) {
            LogFacility::LogMessage("TnmsManager::parseConfig: Error creating agent server socket" 
                    + _agent->errorStr());
            return false;
        }

        _agentId = _evmgr->addIOEvent(_agentHandler, _agent->getFD(), _agent, true);
    } 

    // client server startup
    if ( nsvrcfg.client_port != svrcfg.client_port 
            && nsvrcfg.client_port > 0 )
    {
        LogFacility::Message  logmsg;
        logmsg << "TnmsManager::parseConfig(): Client Server listening on port " 
               << nsvrcfg.client_port;
        LogFacility::LogMessage(logmsg.str());

        if ( _client ) {
            _client->close();
            _evmgr->removeEvent(_clientId);
            _clientId = 0;
        }

        _client = new Socket(0, nsvrcfg.client_port, SOCKET_SERVER, SOCKET_TCP);

        if ( ! _client->init(false) ) {
            LogFacility::LogMessage("TnmsManager::parseConfig: Error creating client server socket" 
                    + _client->errorStr());
            return false;
        }

        if ( _startDelay == 0 )
            _clientId = _evmgr->addIOEvent(_clientHandler, _client->getFD(), _client, true);
    }

    // assign the new config
    _tconfig = config;

    this->destroyClients();
    // init mirror connections
    if ( _tconfig.clients.size() > 0 )
        this->createClients();

    return true;
}


void
TnmsManager::logRotate ( std::string logfile, const time_t & now )
{
    if ( logfile.empty() ) 
        return;

    char        datestr[64];
    struct tm  *ltm   = ::localtime(&now);
    int         today = ltm->tm_yday;

    if ( _today != today ) {
        LogFacility::CloseLogFile(_logname);

        ::strftime(datestr, 64, ".%Y%02m%02d", ltm);

        logfile.append(datestr);
        _logname = logfile;
        _today   = today;

        LogFacility::OpenLogFile(_logname, LogFacility::GetLogPrefix(), logfile, true);
        LogFacility::SetDefaultLogName(_logname);
    }

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

bool
TnmsManager::getDebug() const
{
    return this->_debug;
}

void
TnmsManager::setVerbose ( bool v )
{
    this->_verbose = v;
}

bool
TnmsManager::getVerbose() const
{
    return this->_verbose;
}

const std::string&
TnmsManager::getErrorStr() const
{
    return this->_errstr;
}

std::string
TnmsManager::Version()
{
    std::ostringstream  ostr;
    ostr << _Version << "; tnmsCore: v" << TNMS_VERSION_MAJOR
         << "." << TNMS_VERSION_MINOR;
    return ostr.str();
}


} // namespace

// _TNMSD_TNMSMANAGER_CPP_

