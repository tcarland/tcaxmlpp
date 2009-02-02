#define _TNMSCORE_TNMSBASE_CPP_


#include "TnmsBase.h"


namespace tnmsCore {

// for accepting only human readable strings
#define LOW_CHAR  32
#define HIGH_CHAR 126


template< typename Iterator, typename Value >
Iterator first_out_of_range( Iterator  begin,
                             Iterator  end,
                             Value     low,
                             Value     high )
{
    for ( ; begin != end; ++begin ) {
        if ( *begin < low || *begin > high )
            return begin;
    }

    return end;
};


TnmsBase::TnmsBase ( const std::string & name )
    : _agentName(name)
{}

TnmsBase::TnmsBase (  const std::string & name,
                          const std::string & host,
                          uint16_t            port )
    : _agentName(name),
      _hostName(host),
      _hostPort(port)
{}


TnmsBase::~TnmsBase() {}



bool
TnmsBase::add ( const std::string & name, time_t now )
{
    if ( first_out_of_range(name.begin(), name.end(), LOW_CHAR, HIGH_CHAR) != name.end() )
    {
        // out of range
        return false;
    }

    HeirarchicalStringTree<TreeMetric>::Node  * node = NULL;
    HeirarchicalStringTree::BranchNodeList       nodelist;

    node  = _tree.find(name);
    if ( node != NULL ) {
        // already exists
        return false;
    }
    node =  _tree.insert(name, std::inserter(nodelist, nodelist.begin()));
    if ( node == NULL ) {
        // error inserting
        return false;
    }

    /*  Now to dynamically create new oids */
    TnmsOid      newOid;

    TreeMetric & rootMetric = nodelist.front()->getParent()->getValue();
    TreeMetric & metric     = node->getValue();

    if ( rootMetric.metric.getElementOid().lastValue() == 0 ) {
        // error determining Oid
        return false;
    }

    OidList oidlist = rootMetric.metric.getElementOid().getOidList(); // intentional copy

    HeirarchicalStringTree::BranchNodeList::iterator  nIter;
    for ( nIter = nodelist.begin(); nIter != nodelist.end(); ++nIter ) {
        TreeMetric & nodemetric = (*nIter)->getValue();
        nodemetric.lastId++;
        oidlist.push_back(nodemetric.lastId);
        newOid = TnmsOid(oidlist);
        std::cout << "New oid " << newOid.toString() << " for " 
            << (*nIter)->getAbsoluteName() << std::endl;
        nodemetric.metric = TnmsMetric((*nIter)->getAbsoluteName(), newoid);
    }

    /* Now that our parents have oids, we create the leaf oid */
    metric.lastId++;
    oidlist.push_back(metric.lastId);

    newOid = TnmsOid(oidlist);
    metric.metric = TnmsMetric(node->getAbsoluteName(), newOid);

    std::cout << "New oid " << newOid.toString() << " for " 
        << node->getAbsoluteName() << std::endl;

    _updates.insert(&metric);

    return true;
}


bool
TnmsBase::remove ( const std::string & name )
{
    HeirarchicalStringTree<TreeMetric>::Node  * node = NULL;

    node = _tree.find(name);
    if ( node == NULL )
        return false;

    TreeMetric  & metric = node->getValue();

    UpdateSet::iterator  uIter;
    uIter = std::find(_updates.begin(), updates.end(), &metric);

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

    tree.erase(node, std::inserter(strings, strings.begin()));

    return true;
}


bool
TnmsBase::update ( const std::string & name, time_t  now, uint64_t value, 
                     eValueTypes type, const std::string & data )
{
    HeirarchicalStringTree<TreeMetric>::Node  * node = NULL;

    node = _tree.find(name);
    if ( node == NULL )
        return false;

    TreeMetric  & metric = node->getValue();

    metric.metric.setValue(value);

    _updates.insert(&metric);

    return true;
}


bool
TnmsBase::update ( const std::string & name, time_t  now,
                     const std::string & value, const std::string & data )
{
    HeirarchicalStringTree<TreeMetric>::Node  * node = NULL;

    node = _tree.find(name);
    if ( node == NULL )
        return false;

    TreeMetric  & metric = node->getValue();

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
TnmsBase::reconnect ( time_t secs )
{
    _reconTime = secs;
}


time_t  
TnmsBase::reconnect()
{
    return _reconTime;
}

void    
TnmsBase::max_messages ( int max )
{
    _maxMessages = max;
}


int     
TnmsBase::max_messages()
{
    return _maxMessages;
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
