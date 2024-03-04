/**
  * @file XmlNode.cpp
  *
  * Copyright(c) 2008-2024 Timothy Charlton Arland <tcarland@gmail.com>
  *
  * @section LICENSE
  *
  * This file is part of tcaxmlpp.
  *
  * tcaxmlpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as
  * published by the Free Software Foundation, either version 3 of
  * the License, or (at your option) any later version.
  *
  * tcaxmlpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with tcaxmlpp.
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#define _TCAXMLPP_XMLNODE_CPP_

#include <iostream>
#include <algorithm>

#include "XmlNode.h"
#include "XmlAttribute.h"


namespace tcaxmlpp {


XmlNode::XmlNode()
    : _parent(NULL),
      _node(NULL),
      _type(XML_ELEMENT_NODE),
      _name(),
      _debug(false)
{ }


XmlNode::XmlNode ( XmlNode     * parent,
                   xmlNodePtr    node,
                   bool          recursive_walk )
    : _parent(parent),
      _node(node),
      _type(XML_ELEMENT_NODE),
      _name(),
      _debug(false)
{
    if ( _node ) 
    {
        _type = _node->type;
        if ( _node->name )
            _name = (char*) _node->name;
        if ( _node->content )
            _text = (char*) _node->content;
    }

    if ( _node && recursive_walk ) {
        this->recursiveWalk();
        this->readNodeAttributes();
    }
}


/**  Creates a new, unattached node. */
XmlNode::XmlNode ( XmlNode           * parent,
                   const std::string & name,
                   int                 type )
    : _parent(parent),
      _name(name),
      _debug(false)
{
    _node   = ::xmlNewNode(NULL, (const xmlChar*) _name.c_str());
    _type   = (xmlElementType) type;

    if ( _parent ) {
        ::xmlAddChild(_parent->getNode(), _node);
        _parent->addNode(this);
    }
}

/** XmlNode destructor */
XmlNode::~XmlNode()
{
    XmlNodeIter  nIter;
    XmlAttrIter  aIter;

    for ( nIter = _kids.begin(); nIter != _kids.end(); nIter++ )
    {
        if ( (*nIter) )
            delete *nIter;
    }
    _kids.clear();

    for ( aIter = _attrs.begin(); aIter != _attrs.end(); aIter++ )
    {
        if ( aIter->second )
            delete aIter->second;
    }
    _attrs.clear();

    if ( _node && ! _node->parent )
        ::xmlFreeNode(_node);
}

/** Internal initialization routine for creating XmlNodes */
void
XmlNode::recursiveWalk()
{
    xmlNodePtr  chld;

    for ( chld = _node->children; chld; chld = chld->next )
        this->addNode(new XmlNode(this, chld, true));

    return;
}

/** Internal initialization routine for reading node attributes. */
void
XmlNode::readNodeAttributes()
{
    xmlAttrPtr  attr;

    for ( attr = _node->properties; attr; attr = attr->next )
    {
        XmlAttribute* xa = new XmlAttribute(this, attr);
        _attrs[xa->getKey()] = xa;
    }

    return;
}

/** Return the name of the XmlNode */
std::string
XmlNode::getNodeName() const
{
    return _name;
}

/** Returns the underlying libxml2 xmlNodePtr */
xmlNodePtr
XmlNode::getNode()
{
    return _node;
}

/** Returns the XML node type value */
int
XmlNode::getNodeType() const
{
    return _type;
}

std::string
XmlNode::getNodeContent() const
{
    return _text;
}

void
XmlNode::setNodeContent ( const std::string & txt )
{
    if ( ! txt.empty() )
        ::xmlNodeSetContent(_node, (const xmlChar*) txt.c_str());
}

/** Returns the parent XmlNode or NULL if this node is the root
  * of the document.
 **/
XmlNode*
XmlNode::getParent()
{
    return _parent;
}

void
XmlNode::setParent( XmlNode * parent )
{
    _parent  = parent;
}

bool
XmlNode::hasChildren() const
{
    return(_kids.size() > 0);
}

/** Returns a boolean indicating whether this node has any attributes */
bool
XmlNode::hasAttributes() const
{
    return(_attrs.size() > 0);
}

bool
XmlNode::isRootElement() const
{
    return(_parent != NULL);
}

std::string&
XmlNode::getErrorStr()
{
    return _errStr;
}

/** Finds the first node named 'nodename' and with the given
  * attribute/value pair if provided.
 **/
XmlNode*
XmlNode::findNode ( const std::string & nodename,
                    const std::string & attrkey, const std::string & attrval )
{
    XmlNode     * node = NULL;
    XmlNodeIter   nIter;

    nIter = std::find_if(_kids.begin(), _kids.end(),
                 FindXmlNodeName(nodename, attrkey, attrval));

    if ( nIter == _kids.end() )
        return node;

    node = *nIter;

    return node;
}

/** Finds returns a list of all nodes with the given name and optionally
  * just nodes with the given attribute key.
 **/
XmlNodeList
XmlNode::findNodes ( const std::string & nodename, const std::string & attrkey )
{
    XmlNodeList nodes;
    XmlNodeIter nIter;
    XmlNode   * node = NULL;

    for ( nIter = _kids.begin(); nIter != _kids.end(); ++nIter )
    {
        node = (XmlNode*) *nIter;

        if ( node && node->getNodeName().compare(nodename) == 0 )
        {
            if ( attrkey.empty() )
                nodes.push_back(node);
            else if ( node->hasAttr(attrkey) )
                nodes.push_back(node);
        }
    }

    return nodes;
}


/**  This will add the provided node as a child of the
 *   current object. It does not attach or set heirarchy and
 *   will fail if the node has not already been attached.
 **/
bool
XmlNode::addNode ( XmlNode * node )
{
    if ( node == NULL )
        return false;

    if ( _debug ) {
        printf("XmlNode::addNode(): node: %s\n",
        node->getNodeName().c_str());
        node->setDebug(_debug);
    }

    XmlNodeIter nIter = std::find(_kids.begin(), _kids.end(), node);

    if ( nIter != _kids.end() ) {
        _errStr = "Node already exists";
        return false;
    }

    if ( node->getParent() == this ) {
        _kids.push_back(node);
    } else if ( node->getParent() == NULL ) {
        node->setParent(this);
        _kids.push_back(node);
    } else {
        _errStr = "Node already has parent";
        return false;
    }

    return true;
}


/**  Removes and unattaches the provided child node from
  *  the current object.
 **/
bool
XmlNode::removeNode ( XmlNode * node )
{
    XmlNodeIter  nIter;
    xmlNodePtr   n;

    if ( node == NULL )
        return false;

    nIter = std::find(_kids.begin(), _kids.end(), node);

    if ( nIter == _kids.end() )
        return false;

    n = node->getNode();

    if ( n != NULL )
        ::xmlUnlinkNode(n);

    _kids.erase(nIter);

    return true;
}

/** Finds the attribute associated with provided key. */
XmlAttribute*
XmlNode::findAttr ( const std::string & key )
{
    XmlAttrIter  aIter;

    if ( (aIter = _attrs.find(key)) == _attrs.end() )
        return NULL;

    return((XmlAttribute*) aIter->second);
}

const XmlAttribute*
XmlNode::findAttr ( const std::string & key ) const
{
    XmlAttrMap::const_iterator  cIter;

    cIter = _attrs.find(key);

    if ( cIter == _attrs.end() )
        return NULL;

    return cIter->second;
}

bool
XmlNode::hasAttr ( const std::string & key ) const
{
    if ( this->findAttr(key) == NULL )
        return false;
    return true;
}

bool
XmlNode::removeAttr ( const std::string & key )
{
    XmlAttribute*  attr;
    XmlAttrIter    aIter;

    if ( (aIter = _attrs.find(key)) == _attrs.end() )
	return false;

    attr = (XmlAttribute*) aIter->second;

    ::xmlRemoveProp(attr->getNode());

    _attrs.erase(aIter);
    delete attr;

    return true;
}

bool
XmlNode::addAttr ( const std::string & key, const std::string & val )
{
    XmlAttribute*  attr;

    if ( (attr = this->findAttr(key)) != NULL )
        return false;

    attr = new XmlAttribute(this, key, val);

    _attrs[key] = attr;

    return true;
}

std::string
XmlNode::getAttr ( const std::string & key ) const
{
    const XmlAttribute* attr = this->findAttr(key);

    if ( attr == NULL )
        return std::string("");

    return attr->getValue();
}

void
XmlNode::setAttr ( const std::string & key, const std::string & val )
{
    XmlAttribute*  attr;

    if ( (attr = this->findAttr(key)) != NULL ) {
        attr->setValue(val);
        return;
    }

    attr = new XmlAttribute(this, key, val);
    _attrs[key] = attr;

    return;
}

void
XmlNode::setDebug ( bool d )
{
    this->_debug = d;
}

void
XmlNode::printNode ( bool recursive )
{
    XmlNode*       node = NULL;
    XmlAttribute*  attr = NULL;
    XmlNodeIter    nIter;
    XmlAttrIter    aIter;

    std::cout << "Node: <" << _name << ">" << std::endl;

    for ( aIter = _attrs.begin(); aIter != _attrs.end(); aIter++ )
    {
        attr = (XmlAttribute*) aIter->second;
        if ( attr == NULL )
            continue;
        std::cout << "      " << attr->getKey() << "  =  " << attr->getValue()
                  << std::endl;
    }
    std::cout << " Node has " << _kids.size() << " children\n\n";

    if ( recursive )
    {
        for ( nIter = _kids.begin(); nIter != _kids.end(); nIter++ )
        {
            node = (XmlNode*) *nIter;
            if ( node == NULL )
                continue;
            node->printNode(true);
        }
    }

    return;
}


XmlNode*
XmlNode::RecursiveFindNode ( XmlNode * root,
                             const std::string & name,
                             const std::string & key,
                             const std::string & val )
{
    if ( root->getNodeName().compare(name) == 0
        && root->getAttr(key).compare(val) == 0 )
    {
        return root;
    }

    XmlNodeIter    nIter;
    XmlNodeList  & kids  = root->getNodeList();
    XmlNode      * node  = NULL;

    for ( nIter = kids.begin(); nIter != kids.end(); ++nIter )
    {
        node = (*nIter)->findNode(name, key, val);

        if ( node != NULL )
            break;

        node = XmlNode::RecursiveFindNode(*nIter, name, key, val);

        if ( node != NULL )
            break;
    }

    return node;
}

bool
XmlNode::FindXmlNode::operator() ( const XmlNode * n )
{
    return(this->node == n);
}

bool
XmlNode::FindXmlNodeName::operator() ( const XmlNode * node )
{
    if ( node->getNodeName().compare(name) == 0 )
        return(node->getAttr(key).compare(val) == 0);
    return false;
}

bool
XmlNode::FindXmlNodesByName::operator() ( XmlNode * node )
{
    if ( node->getNodeName().compare(name) == 0 )
        nodes.push_back(node);
    return true;
}

}  // namespace

//  _TCAXMLPP_XMLNODE_CPP_
