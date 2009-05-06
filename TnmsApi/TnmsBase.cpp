#define _TNMSCORE_TNMSBASE_CPP_

#include <sys/stat.h>
#include <iostream>


#include "TnmsBase.h"
#include "TnmsClient.h"
#include "TnmsTree.h"
#include "LogFacility.h"


namespace tnmsCore {

// for accepting only human readable strings
#define LOW_CHAR  32
#define HIGH_CHAR 126


const
std::string TnmsBase::ApiVersion = "0.1";


template< typename Iterator, typename Value >
Iterator first_out_of_range( Iterator  begin,
                             Iterator  end,
                             Value     low,
                             Value     high )
{
    for ( ; begin != end; ++begin ) 
    {
        if ( *begin < low || *begin > high )
            return begin;
    }

    return end;
};


TnmsBase::TnmsBase ( const std::string & name )
    : _tree(new TnmsTree()),
      _agentName(name),
      _conn(new TnmsClient(_tree))
{}

TnmsBase::TnmsBase ( const std::string & name,
                     const std::string & host,
                     uint16_t            port )
    : _tree(new TnmsTree()),
      _agentName(name),
      _conn(new TnmsClient(_tree)),
      _hostName(host),
      _hostPort(port),
      _holddown(0),
      _holddown_interval(30),
      _reconnect(0),
      _reconnect_interval(30),
      _reconfig(0),
      _reconfig_interval(DEFAULT_RECONFIG_INTERVAL),
      _subscribed(false),
      _debug(false)
{}


TnmsBase::~TnmsBase()
{
    delete _tree;
}


int
TnmsBase::send ( time_t  now )
{
    struct tm  * t;
    int          rd;

    if ( now == 0 )
        now = ::time(NULL);
    
    t = ::localtime(&now);
    LogFacility::SetLogTime(now);

    if ( this->checkConfig(now) != 0 )
    {
        LogFacility::LogMessage("TnmsAPI: Invalid Configuration");
        return TNMSERR_CONFIG;
    }

    if ( ! _config.logfile.empty() )
        this->openLog(_config.logfile, now);

    // Maintain connection
    if ( (rd = this->checkConnection(now)) != 0 )
        return rd;

    // Receive messages
    if ( _conn->receive(now) < 0 ) {
        LogFacility::LogMessage("TnmsAPI: connection lost in receive().");
        _conn->close();
        return TNMSERR_CONN_LOST;
    }

    if ( (rd = this->checkSubscription(now)) != 0 )
        return rd;

    // check for conn flush
    if ( _conn->txBytesBuffered() > 0 ) 
    {
        if ( _conn->send() < 0 ) {
            LogFacility::LogMessage("TnmsAPI: connection lost in send().");
            _conn->close();
            return TNMSERR_CONN_LOST;
        }
    }

    // current interval flush
    if ( _holddown <= now ) {
        _holddown = now + _holddown_interval;
        _tree->updateClients();
        _conn->flush();
    }

    // Receive messages
    if ( _conn->receive(now) < 0 ) {
        _conn->close();
        return TNMSERR_CONN_LOST;
    }

    return TNMSERR_NONE;
}



bool
TnmsBase::add ( const std::string & name, 
                const time_t      & now, 
                const std::string & data )
{
    LogFacility::Message  msg;

    if ( first_out_of_range(name.begin(), name.end(), LOW_CHAR, HIGH_CHAR) != name.end() )
    {
        msg << "TnmsAPI ERROR: node name contains invalid characters";
        if ( LogFacility::GetDebug() )
            msg << ": '" << name << "'";
        LogFacility::LogMessage(msg.str());
        return false;
    }

    TnmsMetric  metric;
    if ( _tree->add(name) && ! data.empty() )
    {
        _tree->request(name, metric);
        metric.setPvtData(data);
    }
        
/*
    MetricTree::Node           * node;
    MetricTree::BranchNodeList   nodelist;

    node = _tree->find(name);
    if ( node != NULL ) {
        msg << "TnmsAPI ERROR: node name already exists: '" << name << "'";
        LogFacility::LogMessage(msg.str());
        return false;
    }

    node = _tree->insert(name, std::inserter(nodelist, nodelist.begin()));
    if ( node == NULL ) {
        msg << "TnmsAPI ERROR: insert failed for '" << name << "'";
        LogFacility::LogMessage(msg.str());
        return false;
    }
*/

/*
#ifdef USE_OIDS 
    //  Now to dynamically create new oids
    TnmsOid      newOid;
    TnmsMetric * rootMetric = nodelist.front()->getParent()->getValue();
    TnmsMetric * metric     = node->getValue();

    if ( rootMetric.getElementOid().lastValue() == 0 ) {
        // error determining Oid
        return false;
    }

    OidList  oidlist = rootMetric.getElementOid().getOidList(); // intentional copy
    MetricTree::BranchNodeList::iterator   nIter;

    for ( nIter = nodelist.begin(); nIter != nodelist.end(); ++nIter ) {
        TnmsMetric * nodemetric = (*nIter)->getValue();
        nodemetric.lastId++;
        oidlist.push_back(nodemetric.lastId);
        newOid = TnmsOid(oidlist);
        std::cout << "New oid " << newOid.toString() << " for " 
            << (*nIter)->getAbsoluteName() << std::endl;
        nodemetric.metric = TnmsMetric((*nIter)->getAbsoluteName(), newOid);
    }

    // Now that our parents have oids, we create the leaf oid 
    metric.lastId++;
    oidlist.push_back(metric.lastId);

    newOid = TnmsOid(oidlist);
    metric.metric = TnmsMetric(node->getAbsoluteName(), newOid);

    std::cout << "New oid " << newOid.toString() << " for " 
        << node->getAbsoluteName() << std::endl;
#endif 
*/

   // _updates.insert(&metric);

    return true;
}


bool
TnmsBase::remove ( const std::string & name )
{
    return _tree->remove(name);
}


bool
TnmsBase::update ( const std::string & name, 
                   const time_t      & now, 
                   uint64_t          & value, 
                   eValueType          type, 
                   const std::string & data )
{
    TnmsMetric  metric;

    if ( ! _tree->request(name, metric) )
        return false;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsAPI::update() " + name);

    metric.setValue(type, value);
    _tree->update(metric);

    return true;
}


bool
TnmsBase::update ( const std::string & name, 
                   const time_t      & now,
                   const std::string & value, 
                   const std::string & data )
{    
    TnmsMetric  metric;

    if ( ! _tree->request(name, metric) )
        return false;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsAPI::update() " + name);

    metric.setValue(TNMS_STRING, value);
    _tree->update(metric);

    return true;
}


void
TnmsBase::clear()
{
    //_tree->clear();
    // remove _agentName
}


// to-do: - reference xmlconfig from tnmssocket? 
//        - chksum/version config for quicker check.
//
int
TnmsBase::checkConfig ( const time_t & now )
{
    if ( _config.agent_name.empty() && _agentName.empty() )
        return TNMSERR_CONFIG;

    if ( _agentName.empty() && _xmlConfig.empty() )
    {
        if ( ! _conn->isAuthorized() ) 
            return TNMSERR_NONE;
        else if ( _config.agent_name.empty() && _conn->getConfig().empty() )
            return TNMSERR_CONFIG;
    }
    else if ( _xmlConfig.empty() ) 
    {
        struct stat  configStat;

        if ( ::stat(_configName.c_str(), &configStat) != 0 )
            return TNMSERR_CONFIG;

        if ( configStat.st_mtime != _reconfig ) {
            _reconfig = configStat.st_mtime;
            return this->reconfigure(now);
        }
    } else {
        // validate xmlConfig change?
        // handle config versioning check. could simply .compare() for now 
        // if done infrequently.
    }

    return TNMSERR_NONE;
}


int
TnmsBase::checkConnection ( const time_t & now )
{
    if ( ! _conn->isConnected() || _conn->isConnecting() ) {
        int  con = 0;
        _subscribed = false;

        if ( _reconnect > now )
            return TNMSERR_NO_CONN;

        if ( _conn->isConnecting() )
            con = _conn->openConnection();
        else
            con = _conn->openConnection(_hostName.c_str(), _hostPort);

        if ( con < 0 ) {
            LogFacility::LogMessage("TnmsAPI: failed to establish connection.");
            _reconnect = now + _reconnect_interval;
            return TNMSERR_CONN_FAIL;
        } else if ( con > 0 ) {   // login
            LogFacility::LogMessage("TnmsAPI: connection established.");
            _conn->login(_agentName, "");
            return TNMSERR_NONE;
        } else {                  // in progress
            LogFacility::LogMessage("TnmsAPI: connection in progress.");
            return TNMSERR_NO_CONN;
        }
    }

    return TNMSERR_NONE;
}


int
TnmsBase::checkSubscription ( const time_t & now )
{
    if ( _conn->isAuthorized() && ! this->_subscribed )
    {
        std::string  xmlstr = _conn->getConfig();

        if ( ! xmlstr.empty() && _xmlConfig.compare(xmlstr) != 0 ) {
            _xmlConfig = xmlstr;
            return this->reconfigure(now);
        }

        _subscribed = this->_tree->subscribe("*", _conn);

        /*
        if ( ! this->sendTree(now) ) {
            LogFacility::LogMessage("TnmsAPI: Connection lost sending tree.");
            return TNMSERR_CONN_LOST;
        } else {
            LogFacility::LogMessage("TnmsAPI: Tree sent.");
            _subscribed = true;
        }
        */

    } else if ( ! _conn->isAuthorized() ) {
        if ( _reconnect > now ) {
            return TNMSERR_CONN_DENIED;
        }

        _reconnect   = now + _reconnect_interval;
        _subscribed  = false;

        LogFacility::LogMessage("TnmsAPI: sending credentials.");
        _conn->login(_agentName, "");
    }

    return TNMSERR_NONE;
}


int
TnmsBase::reconfigure ( const time_t & now )
{
    TnmsConfigHandler  configHandler;
    TnmsConfig         oldconfig;

    if ( ! _xmlConfig.empty() ) {
        configHandler = TnmsConfigHandler(_xmlConfig.c_str(), _xmlConfig.length(), 
                                           TNMSAPI_CONFIG_ROOT);
        LogFacility::LogMessage("TnmsAPI::reconfigure using 'network' config.");
    } else {
        configHandler = TnmsConfigHandler(_configName, TNMSAPI_CONFIG_ROOT);
        LogFacility::LogMessage("TnmsAPI::reconfigure using 'local' config.");
    }

    if ( ! configHandler.parse() )
        return TNMSERR_CONFIG;

    oldconfig = _config;
    _config   = configHandler.config;

    //log name
    if ( _config.logfile.compare(oldconfig.logfile) != 0 ) {
        LogFacility::Message  logmsg;

        if ( _config.logfile.empty() ) {
            logmsg << "TnmsAPI::reconfigure closing log to file by request.";
        } else {
            logmsg << "TnmsAPI::reconfigure moving to logfile name " << _config.logfile;
        }

        LogFacility::LogMessage(logmsg.str());
        LogFacility::CloseLogFile();
    }

    this->openLog(_config.logfile, now);

    if ( _config.clients.size() == 0 ) {
        LogFacility::LogMessage("TnmsAPI::reconfigure() ERROR: no connection config");
        return TNMSERR_CONFIG;
    }

    TnmsClientConfig & clientnew = _config.clients.front();
    TnmsClientConfig & clientold = oldconfig.clients.front();

    // agent name
    if ( _config.agent_name.compare(oldconfig.agent_name) != 0 ) {
        _agentName  = _config.agent_name;
        _subscribed = false;
        if ( _conn->isConnected() ) {
            LogFacility::LogMessage("TnmsAPI::reconfigure() agent name modified, resetting.");
            _conn->close();
        }
        _tree->remove(oldconfig.agent_name);
        _tree->add(_agentName);
    }

    // server
    if ( (oldconfig.clients.size() > 0) && 
         (clientnew.hostname.compare(clientold.hostname) != 0 || clientnew.port != clientold.port) )
    {
        if ( _conn->isConnected() ) {
            LogFacility::LogMessage("TnmsAPI::reconfigure connection state change");
            _conn->close();
        }
    }
    _hostName           = clientnew.hostname;
    _hostPort           = clientnew.port;
    _holddown_interval  = clientnew.holddown_interval;
    _reconnect_interval = clientnew.reconnect_interval;

    _conn->flushLimit(clientnew.flush_limit);
    _conn->compression(_config.compression);

    return TNMSERR_NONE;
}


void
TnmsBase::openLog ( const std::string & logfile, const time_t & now )
{
    struct tm * t  = ::localtime(&now);

    if ( LogFacility::IsOpen() && _config.logfile.compare(logfile) == 0 )
    {
        if ( t->tm_yday == _today )
            return;
    }

    if ( LogFacility::IsOpen() && ! LogFacility::GetLogFileName().empty() ) {
        LogFacility::Message  msg;

        if ( logfile.empty() )
            msg << "TnmsAPI logfile name missing, closing logfile";
        else
            msg << "TnmsAPI switching log to " << logfile;

        LogFacility::LogMessage(msg.str());
        LogFacility::CloseLogFile();
    }

    _config.logfile = logfile;

    if ( _config.logfile.empty() )
        return;

    char         line[60];
    std::string  file   = _config.logfile;
    std::string  prefix = "TnmsAPI:";
    _today              = t->tm_yday;

    strftime(line, 60, ".%Y%m%d", t);
    file.append(line);
    prefix.append(_agentName).append(" : ");
    LogFacility::OpenLogFile(prefix, file);

    return;
}


void
TnmsBase::set_config ( const std::string & filename )
{
    _configName = filename;
}


void
TnmsBase::holddown_interval  ( time_t secs )
{
    _holddown = secs;
}


time_t
TnmsBase::holddown_interval() const
{
    return _holddown;
}


void    
TnmsBase::reconnect_interval ( time_t secs )
{
    _reconnect = secs;
}


time_t  
TnmsBase::reconnect_interval() const
{
    return _reconnect;
}

void
TnmsBase::config_interval ( time_t secs )
{
    _reconfig = secs;
}

time_t
TnmsBase::config_interval() const
{
    return _reconfig;
}


void    
TnmsBase::flush_limit ( int max )
{
    _flushLimit = max;
}


int     
TnmsBase::flush_limit() const
{
    return _flushLimit;
}

void    
TnmsBase::set_debug ( bool debug )
{
    _debug = debug;
}


void    
TnmsBase::set_syslog ( int facility )
{
    std::string  prefix = "TnmsAPI:";

    prefix.append(_agentName).append(" : ");
    LogFacility::OpenSyslog(prefix, facility);
    return;
}


void    
TnmsBase::set_logfile ( const std::string & logfilename )
{
    _config.logfile = logfilename;
}


bool    
TnmsBase::need_flush()
{
    return false;
}


size_t  
TnmsBase::flushsize()
{
    return 0;
}


}  // namespace

// _TNMS_TNMSBASE_CPP_

