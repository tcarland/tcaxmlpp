/**
  * @file XmlNode.h
  *    Class for manipulating a node, its children, and 
  * its attributes.
  *
  * Copyright(c) 2008, Timothy Charlton Arland
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
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
#ifndef _TCAXMLPLUS_XMLNODE_H_
#define _TCAXMLPLUS_XMLNODE_H_

#include "XmlDocument.h"


namespace tcaxmlpp {


class XmlNode;
class XmlAttribute;

//-----------------------------------------------------------//
//
typedef std::list<XmlNode*>                     XmlNodeList;
typedef XmlNodeList::iterator                   XmlNodeIter;

typedef std::map<std::string, XmlAttribute*>    XmlAttrMap;
typedef XmlAttrMap::iterator                    XmlAttrIter;

//-----------------------------------------------------------//

/**  The XmlNode object acts as a container for all sub-nodes or 
  *  children. The class ignores xsd type information and handles 
  *  all attributes and node tags as strings. The root level XmlNode
  *  object would provide the primary interface into manipulating 
  *  the xml content.
 **/
class XmlNode {
    friend class XmlDocument;

  public:

    typedef XmlNodeList::iterator        iterator;
    typedef XmlNodeList::const_iterator  const_iterator;
    
  public:

    XmlNode();
    XmlNode   ( XmlNode           * parent, 
                xmlNodePtr          node, 
	        bool                recursive_walk = true );

    XmlNode   ( XmlNode           * parent, 
                const std::string & name, 
	        int                 type = XML_ELEMENT_NODE );

    virtual ~XmlNode();


    xmlNodePtr     getNode();
    xmlNodePtr     getNodePtr()   { return this->getNode(); }

    // node info
    std::string    getNodeName() const;
    int            getNodeType() const;
    XmlNode*       getParent();

    bool           isRootElement() const;

    // children
    inline size_t  children()  const { return _kids.size(); }
    inline size_t  nodeCount() const { return this->children(); }
    bool           hasChildren() const;
    
    bool           addNode           ( XmlNode * node );
    bool           removeNode        ( XmlNode * node );

    XmlNode*	   findNode          ( const std::string & nodename,
                                       const std::string & attrkey = "", 
                                       const std::string & attrval = "" );

    void           setDebug          ( bool d );
    void           printNode         ( bool recursive = false );
    std::string&   getErrorStr();

    // nodelist iterators
    iterator       begin()           { return _kids.begin(); }
    iterator       end()             { return _kids.end(); }
    const_iterator begin() const     { return _kids.begin(); }
    const_iterator end()   const     { return _kids.end(); }
    bool           empty() const     { return( _attrs.empty() && _kids.empty() ); }

    // node and attribute containers
    XmlAttrMap&    getAttributeMap() { return _attrs; }
    XmlAttrMap&    getAttrMap()      { return this->getAttributeMap(); }
    XmlNodeList&   getNodeList()     { return _kids; }

    // attributes
    bool           hasAttributes() const;
    inline size_t  attributes()    const { return _attrs.size(); }
    inline size_t  attrCount()     const { return this->attributes(); }


    const
    XmlAttribute*  findAttr         ( const std::string & name ) const;
    XmlAttribute*  findAttr         ( const std::string & name );

    bool           hasAttr          ( const std::string & name ) const;
    std::string    getAttr          ( const std::string & name ) const;

    bool           removeAttr       ( const std::string & name );
    bool           addAttr          ( const std::string & key, 
                                      const std::string & val );
    void           setAttr          ( const std::string & name, 
                                      const std::string & val );

    // long-name attribute methods
    inline 
    XmlAttribute*  findAttribute    ( const std::string & key )
                                    { return this->findAttr(key); }
    inline 
    bool           hasAttribute     ( const std::string & key ) const
                                    { return this->hasAttr(key); }
    inline 
    bool           haveAttribute    ( const std::string & key ) const
                                    { return this->hasAttr(key); }
    inline 
    bool           haveAttr         ( const std::string & key ) const
                                    { return this->hasAttr(key); }
    inline 
    bool           addAttribute     ( const std::string & key,
                                      const std::string & val )
                                    { return this->addAttr(key, val); }
    inline 
    bool           removeAttribute  ( const std::string & key )
                                    { return this->removeAttr(key); }
    inline 
    std::string    getAttribute     ( const std::string & key ) const
                                    { return this->getAttr(key); }
    inline 
    void           setAttribute     ( const std::string & key, 
                                      const std::string & val )
                                    { return this->setAttr(key, val); }

    static
    XmlNode*       RecursiveFindNode ( XmlNode * root,
                                       const std::string & name,
                                       const std::string & key = "",
                                       const std::string & val = "" );

  public:

    class FindXmlNode {
      public:
        XmlNode * node;

        explicit FindXmlNode ( XmlNode * n ) : node(n) {}

        bool operator() ( const XmlNode * n );
    };

    class FindXmlNodeName {
      public:
        std::string  name;
        std::string  key;
        std::string  val;

        explicit FindXmlNodeName ( const std::string & name_, 
                                   const std::string & key_, 
                                   const std::string & val_ )
           : name(name_), key(key_), val(val_) 
        {}

        bool operator() ( const XmlNode * node );
    };


  protected:

    virtual void   recursiveWalk();
    virtual void   readNodeAttributes();

    void           setParent  ( XmlNode * parent );


  protected:

    XmlNode*            _parent;
    
    xmlNodePtr		_node;
    xmlElementType	_type;

    std::string     	_name;
    std::string     	_errStr;
    
    XmlNodeList		_kids;
    XmlAttrMap		_attrs;

    bool                _debug;

};


}  // namespace

#endif  // _TCAXMLPLUS_XMLNODE_H_
