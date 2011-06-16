#define _POLLAP_POLLERCONFIG_CPP_

#include <iostream>


#include "StringUtils.h"
using namespace tcanetpp;

#include "SnmpSession.h"
#include "SnmpOid.h"
using namespace tcasnmp;


#include "PollerConfig.h"


namespace pollap {


bool
PollerConfig::parseDeviceConfig ( const std::string & filename, PollerDeviceList & devices )
{
    XmlDocument  doc(filename);

    XmlNode *    root = doc.getRootNode();

    if ( root == NULL ) {
        _errstr = "Error reading file ";
        _errstr.append(filename).append(":");
        _errstr.append(doc.getErrorStr());
        return false;
    }

    XmlNode * dnode = root->findNode("devices");

    if ( dnode == NULL ) {
        _errstr = "Invalid xml, <devices> node not found";
        return false;
    }

    XmlNodeList  nodelist = dnode->getNodeList();
    XmlNodeIter  nIter;

    for ( nIter = nodelist.begin(); nIter != nodelist.end(); ++nIter ) {
        XmlNode * node = *nIter;

        if ( node->getNodeName().compare("device") != 0 )
            continue;

        if ( node->getAttribute("name").empty() )
            continue;

        PollerDevice   device;

        device.name      = node->getAttribute("name");
        device.host      = node->getAttribute("host");
        device.community = node->getAttribute("ro_community");

        this->parseDeviceQueries(node, device.queryMap);

        devices.push_back(device);
    }

    return true;
}


bool
PollerConfig::parseDeviceQueries ( XmlNode * node, DeviceQueryMap & devQueryMap )
{
    XmlNodeList  nodelist = node->getNodeList();
    XmlNodeIter  nIter;

    for ( nIter = nodelist.begin(); nIter != nodelist.end(); ++nIter ) 
    {
        XmlNode * node = *nIter;

        if ( node->getNodeName().compare("query") != 0 )
            continue;
        if ( node->getAttribute("name").empty() )
            continue;

        PollerDeviceQuery  devquery;

        devquery.query_name = node->getAttribute("name");

        if ( node->haveAttribute("slot_number") )
            devquery.slot_number = StringUtils::fromString<int>(node->getAttribute("use_slot_num"));

        if ( node->haveAttribute("update_interval") )
            devquery.update_interval = StringUtils::fromString<time_t>(node->getAttribute("update_interval"));

        devQueryMap[devquery.query_name] = devquery;
    }

    return true;
}

    
bool
PollerConfig::parseQueryConfig ( const std::string & filename, PollerQueryMap & queries )
{
    XmlDocument  doc(filename);

    XmlNode *    root = doc.getRootNode();

    if ( root == NULL ) {
        std::cout << "Error reading file " << filename << ":"
            << doc.getErrorStr() << std::endl;
        return false;
    }

    XmlNodeList nodelist = root->getNodeList();
    XmlNodeIter nIter;

    for ( nIter = nodelist.begin(); nIter != nodelist.end(); ++nIter ) 
    {
        XmlNode * node = *nIter;

        if ( node->getNodeName().compare("query_section") != 0 )
            continue;
        if ( node->getAttribute("name").empty() )
            continue;
                    
        PollerQuery   rootQuery;

        rootQuery.name  = node->getAttribute("name");
        rootQuery.key   = ELEMENT_ROOT;
        rootQuery.key.append(rootQuery.name);

        if ( ! parseQuerySection(node, rootQuery) )
            return false;

        queries[rootQuery.name] = rootQuery;
    }

    if ( queries.size() == 0 )
        return false;

    return true;
}


void
PollerConfig::parseQuery ( XmlNode * node, PollerQuery & query )
{
    std::string  soid;

    query.name = node->getAttribute("name");
    query.desc = node->getAttribute("description");
    
    soid = node->getAttribute("oid");

    if ( soid.empty() )
        return;

    query.oid =  SnmpOid(soid);

    if ( node->haveAttribute("oid_is_key") && 
            node->getAttribute("oid_is_key").compare("true") == 0 )
        query.oid_is_key = true;

    if ( node->haveAttribute("use_slot_num") && 
            node->getAttribute("use_slot_num").compare("true") == 0 )
        query.use_slot_num = true;
        
    query.query_type = PollerConfig::GetQueryType(node->getAttribute("query_type"));
        
    if ( node->haveAttribute("refresh") )
        query.refresh = StringUtils::fromString<time_t>(node->getAttribute("refresh"));

    return;
}


bool
PollerConfig::parseQuerySection ( XmlNode * qnode, PollerQuery & rootQuery )
{
    XmlNodeList  nodelist;
    XmlNodeIter  nIter;

    std::cout << "parseQuerySection" << std::endl;

    this->parseQuery(qnode, rootQuery);
    nodelist = qnode->getNodeList();

    for ( nIter = nodelist.begin(); nIter != nodelist.end(); ++nIter ) 
    {
        XmlNode * node = *nIter;

        if ( node == NULL || node->getNodeName().compare("query") != 0 )
            continue;

        PollerQuery  query;

        this->parseQuerySection(node, query);
        rootQuery.subQueries.push_back(query);
    }

    return true;
}


int
PollerConfig::GetQueryType ( const std::string & qtype )
{
    int  result = SNMPQUERY_TYPE_NONE;

    std::cout << "GetQueryType: " << qtype << std::endl; 

    if ( qtype.compare("getnext") == 0 )
        result  = SNMPQUERY_TYPE_GETNEXT;
    else if ( qtype.compare("get") == 0 )
        result  = SNMPQUERY_TYPE_GET;

    return result;
}


} // namespace



