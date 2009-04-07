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

    // Receive messages
    if ( _conn.receive() < 0 ) {
        LogFacility::LogMessage("TnmsAPI: connection lost in receive().");
        _conn.close();
        return TNMSERR_CONN_LOST;
    }

    // dump oids
    if ( _conn.authorized() && ! this->_subscribed )
    {
        std::string  xmlstr = client.getConfig();
        if ( ! xmlstr.empty() && _xmlConfig.compare(xmlstr) != 0 ) {
            _xmlConfig = xmlstr;
            this->reconfigure();
            return TNMSERR_NONE;
        }

        if ( ! this->sendTree(now) ) {
            LogFacility::LogMessage("TnmsAPI: Connection lost sending tree.");
            return TNMSERR_CONN_LOST;
        } else {
            LogFacility::LogMessage("TnmsAPI: Tree sent.");
            _subscribed = true;
            return TNMSERR_NONE;
        }
    } else if ( ! _conn.authorized() ) {
        if ( _reconTime > now ) {
            return TNMSERR_CONN_DENIED;
        }

        _reconTime = now + 30; // need proper val from config here
        _subscribed = false;

        LogFacility::LogMessage("TnmsAPI: sending credentials.");
        _conn.login(_config.agent_name);

        return TNMSERR_NONE;
    }

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
        this->flush();
    }

    // process input
    if ( _conn.receive() < 0 ) {
        _conn.close();
        return TNMSERR_CONN_LOST;
    }

    return TNMSERR_NONE;
}


int
TnmsBase::receive()
{
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
    MetricTree::Node  * node = NULL;

    node = _tree.find(name);
    if ( node == NULL )
        return false;

    TnmsMetric * metric = node->getValue();

    UpdateSet::iterator  uIter;
    uIter = std::find(_updates.begin(), _updates.end(), &metric);

    if ( uIter != _updates.end() ) {
        // withdraw queued update
        _updates.erase(uIter);
    }

    // queue for remove
    // if connected
    //    _removes.insert(node->getAbsoluteName());
    // else
    StringSet            strings;
    StringSet::iterator  sIter;

    _tree.erase(node, std::inserter(strings, strings.begin()));

    return true;
}


bool
TnmsBase::update ( const std::string & name, 
                   const time_t      & now, 
                   uint64_t          & value, 
                   eValueTypes         type, 
                   const std::string & data )
{
    MetricTree::Node  * node = _tree.find(name);
    
    if ( node == NULL )
        return false;

    TnmsMetric  * metric = node->getValue();
    metric.metric.setValue(type, value);
    _updates.insert(&metric);

    return true;
}


bool
TnmsBase::update ( const std::string & name, 
                   const time_t      & now,
                   const std::string & value, 
                   const std::string & data )
{
    MetricTree::Node  * node = NULL;

    node = _tree.find(name);
    if ( node == NULL )
        return false;

    TnmsMetric * metric = node->getValue();

    metric.metric.setValue(value);
    _updates.insert(&metric);

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
            return 1;

        if ( _agentName.empty() && _xmlConfig.empty() ) {

            if ( ! _client.authorized() ) 
                return 0;
            else if ( _config.agent_name.empty() && _client.getNetworkConfig().empty() )
                return 1;

        } else if ( _xmlConfig.empty() ) {
            struct stat  configStat;

            if ( ::stat(_configName.c_str(), &configStat) != 0 )
                return 0;

            if ( configStat.st_mtime != _configTime ) {
                this->reconfigure();
            }
        }
    }

    return 0;
}


void
TnmsBase::reconfigure() {}


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
