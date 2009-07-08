/**
  * XmlNode.cpp
  *  
  * Copyright(c) 2008, Timothy Charlton Arland
  * @Author  tca@charltontechnology.net
  *
  * This file is part of tcaxmlplus.
  *
  * tcaxmlplus is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcaxmlplus is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcaxmlplus.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#define _TCAXMLPLUS_XMLNODE_CPP_

#include <algorithm>

#include "XmlNode.h"


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
    if ( _node ) {
	_type = _node->type;
	if ( _node->name )
	    _name = (char*) _node->name;
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


XmlNode::~XmlNode()
{
    XmlNodeIter  nIter;
    XmlAttrIter  aIter;

    for ( nIter = _kids.begin(); nIter != _kids.end(); nIter++ ) {
	if ( this->_debug ) {
	    printf("Deleting child node %s : %s \n", 
		(*nIter)->getNodeName().c_str(), (*nIter)->getAttr("Name").c_str());
	}
	if ( (*nIter) )
	    delete *nIter;
    }
    _kids.clear();

    for ( aIter = _attrs.begin(); aIter != _attrs.end(); aIter++ ) {
	if ( aIter->second )
	    delete aIter->second;
    }
    _attrs.clear();

    if ( _node && ! _node->parent )
	::xmlFreeNode(_node);
}

void
XmlNode::recursiveWalk()
{
    xmlNodePtr  chld;

    for ( chld = _node->children; chld; chld = chld->next )
        this->addNode(new XmlNode(this, chld));

    return;
}

void
XmlNode::readNodeAttributes()
{
    xmlAttrPtr  attr;

    for ( attr = _node->properties; attr; attr = attr->next ) {
	XmlAttribute* xa = new XmlAttribute(this, attr);
	_attrs[xa->getKey()] = xa;
    }

    return;
}

std::string
XmlNode::getNodeName() const
{
    return _name;
}

xmlNodePtr
XmlNode::getNode()
{
    return _node;
}

int
XmlNode::getNodeType() const
{
    return _type;
}


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

XmlNode*
XmlNode::findNode ( const std::string & nodename,
		    const std::string & attrkey, const std::string & attrval )
{
    XmlNode     *node = NULL;
    XmlNodeIter  nIter;

    nIter = std::find_if(_kids.begin(), _kids.end(), 
	                 FindXmlNodeName(nodename, attrkey, attrval));

    if ( _debug ) {
	printf("XmlNode::findNode(): %s, %s, %s\n", nodename.c_str(),
	    attrkey.c_str(), attrval.c_str());
    }

    if ( nIter == _kids.end() )
	return node;

    node = *nIter;

    return node;
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

XmlAttribute*
XmlNode::findAttr ( const std::string & key )
{
    XmlAttrIter  aIter;

    if ( (aIter = _attrs.find(key)) == _attrs.end() )
	return NULL;

    return((XmlAttribute*) aIter->second);
}

bool
XmlNode::hasAttr ( const std::string & key )
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
XmlNode::getAttr ( const std::string & key )
{
    XmlAttribute* attr = this->findAttr(key);

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

    printf(" node: <%s>\n", _name.c_str());

    for ( aIter = _attrs.begin(); aIter != _attrs.end(); aIter++ ) {
	attr = (XmlAttribute*) aIter->second;

	if ( attr == NULL )
	    continue;

	printf("      %s  =  %s\n", 
	    attr->getKey().c_str(), attr->getValue().c_str());
    }

    printf("Node has %ld children\n\n", _kids.size());

    if ( recursive )
    {
        for ( nIter = _kids.begin(); nIter != _kids.end(); nIter++ ) {
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
        return root;

    XmlNodeIter    nIter;
    XmlNodeList  & kids  = root->getNodeList();
    XmlNode      * node  = NULL;

    for ( nIter = kids.begin(); nIter != kids.end(); ++nIter ) {
        node = (*nIter)->findNode(name, key, val);

        if ( node != NULL )
            break;

        node = XmlNode::RecursiveFindNode(*nIter, name, key, val);

        if ( node != NULL )
            break;
    }

    return node;
}

}  // namespace


//  _TCAXMLPLUS_XMLNODE_CPP_

