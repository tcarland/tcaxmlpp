#define _TNMSCORE_TNMSCONFIG_CPP_


#include "TnmsConfig.h"

#include "XmlDocument.h"
using namespace tcaxmlplus;

#include "StringUtils.h"
#include "CidrUtils.h"
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
{}



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
        result = doc.initDocument(_xmlcfg);

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
        _errstr = "Error finding xml config for: " + _rootname;
        return false;
    }

    XmlNodeList  nlist   = root->getNodeList();
    XmlAttrMap   attrmap = root->getAttrMap();
    XmlNode      * n     = NULL;
    XmlAttribute * attr  = NULL;

    XmlNodeList::iterator  nIter;
    XmlAttrMap::iterator   aIter;

/*
    // our root config node can have any number of dynamic or custom attributes
    // specific to our app, so we blindly parse them out here.
    for ( aIter = attrmap.begin(); aIter != attrmap.end(); ++aIter ) {
        attr = aIter->second;

        if ( attr == NULL ) 
            continue;

        this->_attrs[attr->getKey()] = attr->getValue();
    }
*/
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

    if ( node->haveAttribute("agent_listenport") )
        config.serverConfig.agent_listenport = StringUtils::fromString<uint16_t>(node->getAttribute("agent_listenport"));

    if ( node->haveAttribute("client_listenport") )
        config.serverConfig.agent_listenport = StringUtils::fromString<uint16_t>(node->getAttribute("client_listenport"));
    else   // required? all of our server instances have a client port
        return false;
    
    if ( node->haveAttribute("holddown_interval") )
        config.serverConfig.holddown_interval = StringUtils::fromString<uint32_t>(node->getAttribute("holddown_interval"));
    
    if ( node->haveAttribute("reconnect_interval") )
        config.serverConfig.reconnect_interval = StringUtils::fromString<uint32_t>(node->getAttribute("reconnect_interval"));

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
        clientcfg.hostaddr = CidrUtils::getHostAddr(clientcfg.hostname);
    }

    if ( node->haveAttribute("port") )
        clientcfg.port = StringUtils::fromString<uint16_t>(node->getAttribute("port"));

    // subscribes
    XmlNode   * snode = NULL;
    XmlNodeList nlist = node->getNodeList();
    XmlNodeList::iterator  nIter;
    for ( nIter = nlist.begin(); nIter != nlist.end(); ++nIter ) {
        snode = (XmlNode*) *nIter;

        if ( snode->getNodeName().compare("subscribe") == 0 )
            clientcfg.subs.push_back(snode->getAttribute("name"));
    }

    config.clients.push_back(clientcfg);

    return true;
}

} // namespace

