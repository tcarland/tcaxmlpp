#define _TNMSDB_TNMSARCHIVEMANAGER_CPP_


#include "TnmsArchiveManager.h"

#include "ClientIOHandler.h"
#include "ArchiveClient.h"
#include "ArchiverThread.h"

//tcanetpp
#include "LogFacility.h"
#include "FileUtils.h"


namespace tnmsdb {


std::string
TnmsArchiveManager::_Version = "v0.159";


TnmsArchiveManager::TnmsArchiveManager ( const std::string & configfile )
    : _evmgr(new tcanetpp::EventManager()),
      _sql(NULL),
      _reportId(0),
      _logId(0),
      _clientHandler(new ClientIOHandler()),
      _lastTouched(0),
      _reportDelay(DEFAULT_REPORT_INTERVAL),
      _logCheck(LOG_CHECK_INTERVAL),
      _holddown(DEFAULT_HOLDDOWN_INTERVAL),
      _today(0),
      _configfile(configfile),
      _hup(false),
      _usr(false),
      _debug(false)
{}


TnmsArchiveManager::~TnmsArchiveManager()
{
    this->destroyClients();
    delete _clientHandler;
    if ( _sql )
        delete _sql;
    delete _evmgr;
}


void
TnmsArchiveManager::run()
{
    time_t  now = ::time(NULL);

    LogFacility::InitThreaded();
    LogFacility::SetLogTime(now);

    if ( _debug ) {
        LogFacility::OpenLogStream("stdout", "", &std::cout);
        LogFacility::LogMessage("TnmsArchiveManager::run()");
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

    // create DB thread

    _evmgr->eventLoop(); // main loop

    if ( _debug )
        LogFacility::RemoveLogStream("stdout");

    LogFacility::CloseLogFacility();

    return;
}


void
TnmsArchiveManager::timeout ( const EventTimer & timer )
{
    const time_t & now = timer.abstime.tv_sec;

    LogFacility::SetLogTime(now);

    //check for logrotate
    if ( timer.evid == _logId ) {
        if ( ! _tconfig.syslog )
            this->logRotate(_tconfig.logfile, now);
        return;
    }

    // internal metrics
    if ( timer.evid == _reportId ) {
        LogFacility::LogMessage("TnmsArchiveManager::run()"); // <<add some stats
        return;
    }

    // re-parse config signal
    if ( _hup ) { 
        if ( ! this->parseConfig(_configfile, now) )
            LogFacility::LogMessage("Config error");
        _hup = false;
    }

    if ( _usr ) { // special function
        //_tree->debugDump();
        _usr = false;
    }

    // give time to our handlers
    _clientHandler->timeout(timer);

    return;
}


void
TnmsArchiveManager::createClients()
{
    ArchiveClient * client = NULL;
    evid_t          id     = 0;

    ClientConfigList  & clist  = _tconfig.clients;
    ClientConfigList::iterator  cIter;

    if ( _debug )
        LogFacility::LogMessage("TnmsArchiveManager::createClients()");

    for ( cIter = clist.begin(); cIter != clist.end(); ++cIter ) {
        TnmsClientConfig & cfg  = *cIter;
        std::string      & name = cfg.connection_name;

        ArchiverMap::iterator aIter = _archiverMap.find(name);
        if ( aIter == _archiverMap.end() ) {
            LogFacility::LogMessage("Error: Archiver not found for " + name);
            continue;
        }

        client = new ArchiveClient(aIter->second, cfg);

        // set client attributes
        client->setCompression(_tconfig.compression);
        client->setReconnectTime(cfg.reconnect_interval);
        _clientHandler->addMirror(client);

        if ( client->openConnection(cIter->hostname, cIter->port) < 0 )
            continue;

        id = _evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);

        client->login(_tconfig.agent_name, _tconfig.agent_key);

        MirrorConnection  mirror(id, *cIter, client);
        _clients[cIter->connection_name] = mirror;
    }

    return;
}


void
TnmsArchiveManager::destroyClients ()
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


void
TnmsArchiveManager::createArchivers()
{
    ArchiverDbMap::iterator   dIter;
    SchemaConfigList::iterator sIter;

    if ( _sql == NULL )
    {
        LogFacility::LogMessage("Error with SQL Handle");
        return;
    }

    for ( dIter = _dbConfigMap.begin(); dIter != _dbConfigMap.end(); ++dIter )
    {
        SchemaConfigList & dblist = dIter->second;
        ArchiverSet  archivers;
        for ( sIter = dblist.begin(); sIter != dblist.end(); ++sIter )
        {
            SchemaConfig & schema = *sIter;
            ArchiverThread * archiver = new ArchiverThread(_evmgr, _sql, schema);
            archiver->start();
            archivers.insert(archiver);
        }
        _archiverMap[dIter->first]  = archivers;
    }
            
    return;
}


void
TnmsArchiveManager::destroyArchivers()
{
    ArchiverMap::iterator  aIter;
    ArchiverSet::iterator  sIter;

    for ( aIter = _archiverMap.begin(); aIter != _archiverMap.end(); ++aIter )
    {
        for ( sIter = aIter->second.begin(); sIter != aIter->second.end(); ++sIter )
        {
            ArchiverThread * archiver = (ArchiverThread*) *sIter;

            if ( archiver && archiver->isRunning() )
            {
                archiver->setAlarm();
                archiver->notify();
                archiver->stop();
                delete archiver;
            }
        }
    }

    _archiverMap.clear();
}



bool
TnmsArchiveManager::parseConfig ( const std::string & cfg, const time_t & now )
{
    if ( _lastTouched > 0 && _lastTouched == FileUtils::LastTouched(cfg) )
        return true;

    ArchiveConfigHandler   cfgmgr(cfg, TNMSARCHIVER_CONFIG_ROOT);
    std::string            prefix    = TNMSARCHIVER_CONFIG_ROOT;

    if ( ! cfgmgr.parse() ) {
        if ( LogFacility::IsOpen() ) {
            LogFacility::LogMessage("TnmsArchiveManager::parseConfig() error: " 
                    + cfgmgr.getErrorStr());
        } else {
            fprintf(stderr, "TnmsArchiveManager::parseConfig() error: %s\n",
                    cfgmgr.getErrorStr().c_str());
        }
        return false;
    }

    TnmsConfig & config = cfgmgr.config;

    if ( ! config.agent_name.empty() )
        prefix = config.agent_name;

    LogFacility::SetDefaultLogPrefix(prefix);

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

    ArchiveDbConfig  dbcfg;

    dbcfg.db_name = cfgmgr.getAttribute("dbname");
    dbcfg.db_user = cfgmgr.getAttribute("dbuser");
    dbcfg.db_pass = cfgmgr.getAttribute("dbpass");
    dbcfg.db_host = cfgmgr.getAttribute("dbhost");
    dbcfg.db_port = cfgmgr.getAttribute("dbport");

    if ( dbcfg.db_host.compare(_dbCfg.db_host) != 0 ||
         dbcfg.db_port.compare(_dbCfg.db_port) != 0 || 
         dbcfg.db_name.compare(_dbCfg.db_name) != 0 ||
         dbcfg.db_user.compare(_dbCfg.db_user) != 0 ||
         dbcfg.db_pass.compare(_dbCfg.db_pass) != 0 )
    {
        if ( _sql )
            delete _sql;

        _sql = new SqlSession(dbcfg.db_name, dbcfg.db_user, dbcfg.db_pass, dbcfg.db_host, dbcfg.db_port);
    }

    // assign the new config
    _tconfig     = config;
    _dbConfigMap = cfgmgr.dbConfigMap;
    _dbCfg       = dbcfg;

    // reset
    this->destroyClients();
    this->destroyArchivers();

    // init archivers
    this->createArchivers();

    // init mirror connections
    if ( _tconfig.clients.size() > 0 )
        this->createClients();

    return true;
}


void
TnmsArchiveManager::logRotate ( std::string logfile, const time_t & now )
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
TnmsArchiveManager::setAlarm()
{
    this->_evmgr->setAlarm();
}


void
TnmsArchiveManager::setHUP()
{
    this->_hup = true;
}


void
TnmsArchiveManager::setUSR()
{
    this->_usr = true;
}


void
TnmsArchiveManager::setDebug ( bool d )
{
    this->_debug = d;
}

bool
TnmsArchiveManager::getDebug() const
{
    return this->_debug;
}


const std::string&
TnmsArchiveManager::getErrorStr() const
{
    return this->_errstr;
}

std::string
TnmsArchiveManager::Version()
{
    std::ostringstream  ostr;
    ostr << "tnmsarchive " << _Version << "; tnmsCore Version=" << TNMS_VERSION_MAJOR
         << "." << TNMS_VERSION_MINOR;
    return ostr.str();
}


} // namespace

// _TNMSDB_TNMSARCHIVEMANAGER_CPP_

