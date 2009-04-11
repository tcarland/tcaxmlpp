#define _TNMSCORE_TNMSBASE_CPP_

#include <iostream>


#include "TnmsBase.h"
#include "TnmsAgent.h"

#include "TnmsMetric.h"
#include "TnmsOid.h"



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
    : _agentName(name)
{}

TnmsBase::TnmsBase ( const std::string & name,
                     const std::string & host,
                     uint16_t            port )
    : _agentName(name),
      _hostName(host),
      _hostPort(port),
      _subtree(false),
      _debug(false)
{}


TnmsBase::~TnmsBase() {}


int
TnmsBase::send ( time_t  now )
{
    struct tm  * t;
    int          rd;

    if ( now == 0 )
        now = ::time(NULL);
    
    t = ::localtime(&now);
    LogFacility::SetLogTime(now);

    if ( this->checkConfig() != 0 )
    {
        LogFacility::LogMessage("TnmsAPI: Invalid Configuration");
        return TNMSERR_CONFIG;
    }

    if ( ! _config.logfile.empty() )
        this->openLog(_config.logfile, now);

    // Maintain connection
    if ( (rd = this->checkConnection()) != 0 )
        return rd;

    // Receive messages
    if ( _conn.receive(now) < 0 ) {
        LogFacility::LogMessage("TnmsAPI: connection lost in receive().");
        _conn.close();
        return TNMSERR_CONN_LOST;
    }

    if ( (rd = this->checkSubscription()) != 0 )
        return rd;

    // check for conn flush
    if ( _conn.flushAvailable() > 0 ) 
    {
        if ( _conn.send() < 0 ) {
            LogFacility::LogMessage("TnmsAPI: connection lost in send().");
            _conn.close();
            return TNMSERR_CONN_LOST;
        }
    }

    // current interval flush
    if ( _holddown <= now ) {
        _holddown = now + _config.holddown;
        _tree->updateClients();
        this->flush();
    }

    // Receive messages
    if ( _conn.receive(now) < 0 ) {
        _conn.close();
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

    MetricTree::Node           * node;
    MetricTree::BranchNodeList   nodelist;

    node = _tree.find(name);
    if ( node != NULL ) {
        msg << "TnmsAPI ERROR: node name already exists: '" << name << "'";
        LogFacility::LogMessage(msg.str());
        return false;
    }

    node = _tree.insert(name, std::inserter(nodelist, nodelist.begin()));
    if ( node == NULL ) {
        msg << "TnmsAPI ERROR: insert failed for '" << name << "'";
        LogFacility::LogMessage(msg.str());
        return false;
    }

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

    _updates.insert(&metric);

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
                   eValueTypes         type, 
                   const std::string & data )
{
    TnmsMetric  metric;

    if ( ! _tree->request(name, metric) )
        return false;

    if ( LogFacility::Debug() )
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

    if ( LogFacility::Debug() )
        LogFacility::LogMessage("TnmsAPI::update() " + name);

    metric.setValue(value);
    _tree->update(metric);

    return true;
}


void
TnmsBase::clear()
{
    //_tree->clear();
    // remove _agentName
}

int
TnmsBase::checkConfig()
{
    if ( _configTime <= now )
    {
        _configTime = now + _checkConfigTime;

        if ( _config.agent_name.empty() && _agentName.empty() )
            return TNMSERR_CONFIG;

        if ( _agentName.empty() && _xmlConfig.empty() )
        {
            if ( ! _client.authorized() ) 
                return TNMSERR_NONE;
            else if ( _config.agent_name.empty() && _client.getNetworkConfig().empty() )
                return TNMSERR_CONFIG;
        }
        else if ( _xmlConfig.empty() ) 
        {
            struct stat  configStat;

            if ( ::stat(_configName.c_str(), &configStat) != 0 )
                return TNMSERR_NONE;

            if ( configStat.st_mtime != _configTime ) 
                return this->reconfigure();
        }
    }

    return TNMSERR_NONE;
}


int
TnmsBase::checkConnection()
{
    if ( ! _conn.IsConnected() || _conn.IsConnecting() ) {
        int  con = 0;
        _subscribed = false;

        if ( _reconnect > now )
            return TNMSERR_NO_CONN;

        if ( _conn.IsConnecting() )
            con = _conn.Open();
        else
            con = _conn.Open(_hostName.c_str(), _hostPort);

        if ( con < 0 ) {
            LogFacility::LogMessage("TnmsAPI: failed to establish connection.");
            _reconnect = now + _config.reconnect_interval;
            return TNMSERR_CONN_FAIL;
        } else if ( con > 0 ) {   // login
            LogFacility::LogMessage("TnmsAPI: connection established.");
            _conn.login(_config.agent_name);
            return TNMSERR_NONE;
        } else {                  // in progress
            LogFacility::LogMessage("TnmsAPI: connection in progress.");
            return TNMSERR_NO_CONN;
        }
    }

    return TNMSERR_NONE;
}


int
TnmsBase::checkSubscription()
{
    if ( _conn.authorized() && ! this->_subscribed )
    {
        std::string  xmlstr = _conn.getConfig();

        if ( ! xmlstr.empty() && _xmlConfig.compare(xmlstr) != 0 ) {
            _xmlConfig = xmlstr;
            return this->reconfigure();
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

    } else if ( ! _conn.authorized() ) {
        if ( _reconTime > now ) {
            return TNMSERR_CONN_DENIED;
        }

        _reconTime  = now + 30; // need proper val from config here
        _subscribed = false;

        LogFacility::LogMessage("TnmsAPI: sending credentials.");
        _conn.login(_config.agent_name);
    }

    return TNMSERR_NONE;
}


int
TnmsBase::reconfigure()
{
    TnmsConfigHandler  configHandler;

    if ( ! _xmlConfig.empty() ) {
        configHandler = TnmsConfigHandler(_xmlConfig.c_str(), _xmlConfig.length(), TNMS_CONFIG_ROOT);
        LogFacility::LogMessage("TnmsAPI::reconfigure using 'network' config.");
    } else {
        configHandler = TnmsConfigHandler(_configName, TNMS_CONFIG_ROOT);
        LogFacility::LogMessage("TnmsAPI::reconfigure using 'local' config.");
    }

    TnmsConfig oldconfig = _config;

    if ( ! configHandler.parse() )
        return TNMSERR_CONFIG;

    _config = configHandler.config;

    //log name
    if ( _config.logfile.compare(oldconfig.logfile) != 0 ) {
        LogFacility::Message  logmsg;

        if ( _config.logfile.empty() ) {
            msg << "TnmsAPI::reconfigure closing log to file by request.";
        } else {
            msg << "TnmsAPI::reconfigure moving to logfile name " << _config.logfile;
        }

        LogFacility::LogMessage(logmsg.str());
        LogFacility::Close();
    }

    this->openLog(_config.logfile);

    if ( _config.clients.size() == 0 ) {
        LogFacility::LogMessage("TnmsAPI::reconfigure() ERROR: no connection config");
        return TNMSERR_CONFIG;
    }

    TnmsClientConfig & clientnew = _config.clients.front();
    TnmsClientConfig & clientold = oldconfig.clients.front();

    // agent name
    if ( _config.agent_name.compare(oldconfig.agent_name) != 0 ) {
        if ( _conn->isConnected() )
            LogFacility::LogMessage("TnmsAPI::reconfigure() agent name modified, resetting.");
        _tree->remove(oldconfig.agent_name);
        _conn->close();
        _subscribed = false;
        _agentName = _config.agent_name;
        _tree->add(_agentName);
    }

    // server
    if ( clientcfg.hostname.compare(clientold.hostname) != 0 || clientcfg.port != clientold.port )
    {
        if ( _conn->isConnected() )
            LogFacility::LogMessage("TnmsAPI::reconfigure connection state change");
        _conn.close();
        _hostName = clientcfg.hostname;
        _hostPort = clientcfg.port;
    }

    _conn->flushLimit(clientcfg.flush_limit);
    _conn->compression(_config.compression);

    return TNMSERR_NONE;
}


void
TnmsBase::openLog ( const std::string & logfile, const time_t & now )
{
    struct tm * t  = ::localtime(&now);

    if ( LogFacility::IsOpen() && _config.logfile.compare(logfile) == 0 )
    {
        if ( _t->tm_yday == _today )
            return;
    }

    if ( LogFacility::IsOpen() ) {
        LogFacility::Message  msg;

        if ( logfile.empty() )
            msg << "TnmsAPI logfile name missing, closing logfile";
        else
            msg << "TnmsAPI switching log to " << logfile;

        LogFacility::LogMessage(msg.str());
        LogFacility::Close();
    }

    _config.logfile = logfile;

    if ( _config.logfile.empty() )
        return;

    char         line[60];
    std::string  file = config.logfile;
    _today            = t->tm_yday;

    strftime(line, 60, ".%Y%m%d", t);
    file.append(line);
    LogFacility::OpenLogFile(file);

    return;
}


bool
TnmsBase::setConfig ( const std::string & filename )
{
    _configName = filename;
}


void
TnmsBase::holddown  ( time_t secs )
{
    _holddown = secs;
}


time_t
TnmsBase::holddown()
{
    return _holddown;
}


void    
TnmsBase::reconnect_interval ( time_t secs )
{
    _reconTime = secs;
}


time_t  
TnmsBase::reconnect_interval()
{
    return _reconTime;
}

void    
TnmsBase::flush_limit ( int max )
{
    _flushLimit = max;
}


int     
TnmsBase::flush_limit()
{
    return _flushLimit;
}

void    
TnmsBase::debug ( bool debug )
{
    _debug = debug;
}


void    
TnmsBase::syslog ( int facility )
{}


void    
TnmsBase::logfile ( const std::string & logfilename )
{}


bool    
TnmsBase::need_flush()
{}


size_t  
TnmsBase::flushsize()
{}



}  // namespace

// _TNMSCORE_TNMSBASE_CPP_
