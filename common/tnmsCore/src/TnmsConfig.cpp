/*
 *  TnmsConfig.cpp
 *
 *  Copyright(c) 2008, Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#define _TNMSCORE_TNMSCONFIG_CPP_


#include "TnmsConfig.h"

#include "XmlDocument.h"
using namespace tcaxmlpp;

#include "StringUtils.h"
#include "AddrInfo.h"
#include "LogFacility.h"
using namespace tcanetpp;



namespace tnmsCore {


TnmsConfigHandler::TnmsConfigHandler ( const std::string & xmlfilename,
                                       const std::string & rootname )
    : _xmlcfg(xmlfilename),
      _isXML(false),
      _debug(false),
      _rootname(rootname)
{}


TnmsConfigHandler::TnmsConfigHandler ( const char * xmlblob, size_t len,
                                       const std::string & rootname )
    : _isXML(true),
      _debug(false),
      _rootname(rootname)
{
    _xmlcfg.assign(xmlblob, len);    
}



TnmsConfigHandler::~TnmsConfigHandler() {}



bool
TnmsConfigHandler::parse()
{
    XmlDocument   doc;
    XmlNode     * node = NULL;

    // if DTD then doc.setDTDFile()
    //
    bool result  = false;

    if ( _isXML )
        result = doc.readMemory(_xmlcfg.c_str(), _xmlcfg.length());
    else
        result = doc.initDocument(_xmlcfg, false);

    if ( ! result ) {
        _errstr = doc.getErrorStr();
        return result;
    }

    node = doc.getRootNode();

    if ( node == NULL ) {
        _errstr = "Fatal error parsing configuration, no XML node found";
        return false;
    }

    std::string  tnmsroot = TNMS_CONFIG_ROOT;

    if ( node->getNodeName().compare(tnmsroot) != 0 )
        node = this->findRootNode(node, tnmsroot);

    if ( node == NULL ) {
        _errstr = "Error: xml not recognized";
        return false;
    }
    
    return this->parseRoot(node);
}


XmlNode*
TnmsConfigHandler::findRootNode ( XmlNode * node, std::string & name )
{
    XmlNode     * root = NULL;

    if ( node == NULL )
        return root;

    XmlNodeList   nlist = node->getNodeList();
    XmlNodeList::iterator  nIter;

    for ( nIter = nlist.begin(); nIter != nlist.end(); ++nIter ) {
        root = (XmlNode*) *nIter;

        if ( root == NULL )
            continue;

        if ( root->getNodeName().compare(name) == 0 ) {
            break;
        } else {
            root = this->findRootNode(root, name);

            if ( root != NULL )
                break;
        }
    }

    return root;
}


bool
TnmsConfigHandler::parseRoot ( XmlNode * node )
{
    XmlNode  * root   = NULL;
    bool       result = false;

    root = this->findRootNode(node, _rootname);

    if ( root == NULL ) {
        _errstr = "Error finding xml config for " + _rootname;
        return result;
    }

    XmlNodeList  nlist   = root->getNodeList();
    XmlAttrMap   attrmap = root->getAttrMap();
    XmlNode      * n     = NULL;
    XmlAttribute * attr  = NULL;

    XmlNodeList::iterator  nIter;
    XmlAttrMap::iterator   aIter;

    // first collect our known root elements
    config.agent_name  = root->getAttribute("agent_name");
    config.agent_key   = root->getAttribute("agent_key");
    config.logfile     = root->getAttribute("logfile");
    config.auth_server = root->getAttribute("auth_server");

    if ( root->haveAttribute("auth_port") )
        config.auth_port = StringUtils::fromString<uint16_t>(root->getAttribute("auth_port"));
    if ( root->haveAttribute("version") )
        config.auth_port = StringUtils::fromString<uint32_t>(root->getAttribute("version"));

    if ( root->hasAttr("syslog") && root->getAttr("syslog").compare("true") == 0 )
        config.syslog = true;
    if ( root->hasAttr("debug") && root->getAttr("debug").compare("true") == 0 )
        config.debug = true;

    // map out all the root elements so we keep any custom attributes in hand.
    for ( aIter = attrmap.begin(); aIter != attrmap.end(); ++aIter ) {
        attr = aIter->second;
        if ( attr == NULL ) 
            continue;
        this->_rootAttrs[attr->getKey()] = attr->getValue();
    }

    result = true;
    // now parse our expected config subsections
    for ( nIter = nlist.begin(); nIter != nlist.end(); ++nIter ) {
        n = (XmlNode*) *nIter;
        if ( n == NULL )
            continue;

        if ( n->getNodeName().compare("server") == 0 ) 
            result = this->parseServer(n);
        else if ( n->getNodeName().compare("client") == 0 )
            result = this->parseClient(n);

        if ( ! result )
            break;
    }

    return result;
}


bool
TnmsConfigHandler::parseServer ( XmlNode * node )
{
    if ( node == NULL )
        return false;

    if ( node->haveAttribute("agent_port") )
        config.serverConfig.agent_port = StringUtils::fromString<uint16_t>(node->getAttribute("agent_port"));

    if ( node->haveAttribute("client_port") )
        config.serverConfig.client_port = StringUtils::fromString<uint16_t>(node->getAttribute("client_port"));
    else   // required? all of our server instances have a client port
        return false;
    
    if ( node->haveAttribute("holddown") )
        config.serverConfig.holddown_interval = StringUtils::fromString<uint32_t>(node->getAttribute("holddown"));
    
    if ( node->haveAttribute("reconnect") )
        config.serverConfig.reconnect_interval = StringUtils::fromString<uint32_t>(node->getAttribute("reconnect"));

    if ( this->_debug ) {
        LogFacility::Message  logmsg;
        logmsg << "TnmsConfigHandler::parseServer()  agent port: " 
               << config.serverConfig.agent_port        << " client port: " 
               << config.serverConfig.client_port       << " holddown: " 
               << config.serverConfig.holddown_interval << " reconnect interval: " 
               << config.serverConfig.reconnect_interval;
        LogFacility::LogMessage(logmsg.str());
    }

    return true;
}


bool
TnmsConfigHandler::parseClient ( XmlNode * node )
{
    TnmsClientConfig  clientcfg;

    if ( node == NULL )
        return false;

    if ( node->haveAttribute("name") )
        clientcfg.connection_name = node->getAttribute("name");
    if ( node->haveAttribute("host") ) {
        clientcfg.hostname = node->getAttribute("host");
        clientcfg.hostaddr = AddrInfo::GetHostAddr(clientcfg.hostname);
    }
    if ( node->haveAttribute("port") )
        clientcfg.port = StringUtils::fromString<uint16_t>(node->getAttribute("port"));

    // subscribes
    XmlNode   * snode = NULL;
    XmlNodeList nlist = node->getNodeList();

    XmlNodeList::iterator  nIter;
    for ( nIter = nlist.begin(); nIter != nlist.end(); ++nIter ) 
    {
        snode = (XmlNode*) *nIter;
        if ( snode->getNodeName().compare("subscribe") == 0 ) {
            Subscription sub;
            sub.name = snode->getAttribute("name");
            if ( snode->haveAttribute("depth") )
                sub.depth = StringUtils::fromString<uint32_t>(snode->getAttribute("depth"));
            clientcfg.subs.push_back(sub);
        }
    }

    config.clients.push_back(clientcfg);

    if ( this->_debug ) {
        LogFacility::Message  logmsg;
        logmsg << "TnmsConfigHandler::parseClient()  name: " 
               << clientcfg.connection_name << " host: " 
               << clientcfg.hostname        << " addr: " 
               << clientcfg.hostaddr        << " port: " 
               << clientcfg.port;
        LogFacility::LogMessage(logmsg.str());
    }

    return true;
}


std::string         
TnmsConfigHandler::getAttribute  ( const std::string & key )
{
    std::string   val;

    AttributeMap::iterator kIter = _rootAttrs.find(key);

    if ( kIter == _rootAttrs.end() )
        return val;

    val = kIter->second;

    return val;
}


bool                
TnmsConfigHandler::haveAttribute ( const std::string & key )
{
    AttributeMap::iterator kIter = _rootAttrs.find(key);

    if ( kIter == _rootAttrs.end() )
        return false;

    return true;
}


} // namespace

// _TNMSCORE_TNMSCONFIG_CPP_

