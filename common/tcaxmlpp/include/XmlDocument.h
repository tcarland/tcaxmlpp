/**
  * @file XmlDocument.h
  *  
  *    This class provides an interface for parsing and 
  *  holding a XML document structure using libxml2's DOM api. 
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
#ifndef XmlDocument_h
#define XmlDocument_h


#ifdef WIN32
typedef unsigned u_int;
#else
#include <inttypes.h>
#endif

#include <string>
#include <list>
#include <map>

#include <libxml/tree.h>


#include "XmlNode.h"
#include "XmlAttribute.h"



namespace tcaxmlpp {


class XmlNode;
class XmlAttribute;


#define DEFAULT_ROOT_TAGNAME "tcaxmlpp"
#define DEFAULT_ROOT_VERSION "1.1"
#define MAX_ERR_LINE 256


/**  The XmlNodeFactory interface is used to initialize the root node
 *   as the intended derived type. This allows for XmlNode inheritance
 *   and control of the subsequent node parsing/creation.
 **/
class XmlNodeFactory {
  public:
    virtual ~XmlNodeFactory() {}

    virtual XmlNode*  operator() ( XmlNode    * parent,
                                   xmlNodePtr   node,
                                   bool         recursive_walk = true ) = 0;
};



/**  XmlDocument provides an object-oriented interface to a xml
  *  document via libxml2. The XmlDocument interface primarily 
  *  handles the input/output of well-formed xml fragments or documents. 
  *  Most XML manipulation occurs using the XmlNode class.
 **/
class XmlDocument {

    class DefNodeFactory : public XmlNodeFactory {
      public:
        virtual ~DefNodeFactory() {}
        virtual XmlNode*  operator() ( XmlNode    * parent,
                                       xmlNodePtr   node,
                                       bool         recursive_walk = true );
    };

  public:

    XmlDocument();
    XmlDocument    ( const char * xmlfrag, size_t len );
    XmlDocument    ( const std::string & filename, bool create = false );

    virtual  ~XmlDocument();


    bool                 initDocument   ( const std::string & filename, 
                                          bool create = false );
    bool                 docIsValid() const;

    virtual bool         saveDocument();
    virtual void         clearDocument();

    virtual bool         readMemory     ( const char * xmlblob, size_t len );
    virtual bool         readFile       ( const std::string & filename );
    virtual bool         writeFile      ( const std::string & filename );

    virtual void         setDTDFile     ( const std::string & filename );

    virtual bool         attachNode     ( XmlNode * parent, XmlNode * node );

    XmlNode*             getRootNode();
    XmlNode*        	 setRootNode    ( XmlNode * node, bool erase = true );
    void                 setRootTagName ( const std::string & tagname );

    void                 setNodeFactory ( XmlNodeFactory * nodeFactory );

    void                 setDebug       ( bool d );
    const std::string&   getErrorStr()    const;
    
    std::string          NodeToString   ( XmlNode * node );


    static bool          IsReadable     ( const std::string & filename );
    static const char*   Version();


  protected:

    virtual bool         initParser     ( const std::string & filename );
    virtual bool         initParser     ( const char * fragment, size_t len );

    virtual bool         validate();

    static  void         LibInit();


  protected:

    std::string	        _xmlfile;
    std::string	        _dtdfile;
    std::string         _encoding;
    std::string	        _errStr;
    std::string         _roottag;

    xmlDocPtr           _doc;
    XmlNode*            _root;
    XmlNodeFactory*     _nodeFactory;
    bool                _debug;

    static bool         _Libinit;
    static const char*  _Version;

};


}  // namespace


#endif  /*  XmlDocument_h  */

