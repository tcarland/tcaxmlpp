/**
  * @file XmlDocument.cpp
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
#define _TCAXMLPLUS_XMLDOCUMENT_CPP_

#include <fstream>
#include "XmlDocument.h"

#include <libxml/parser.h>



namespace tcaxmlpp {


bool
XmlDocument::_Libinit = false;

const char*
XmlDocument::_Version = "1.0.4";


//-------------------------------------------------------------//

XmlNode*
XmlDocument::DefNodeFactory::operator() ( XmlNode     * parent, 
                                          xmlNodePtr    node, 
                                          bool          recursive_walk )
{
     return ( new XmlNode(parent, node, recursive_walk) );
}

//-------------------------------------------------------------//

XmlDocument::XmlDocument()
    : _roottag(DEFAULT_ROOT_TAGNAME),
      _doc(NULL),
      _root(NULL),
      _nodeFactory(new DefNodeFactory()),
      _debug(false)
{
    if ( ! XmlDocument::_Libinit )
        XmlDocument::LibInit();
}


XmlDocument::XmlDocument ( const char * xmlfrag, size_t len )
    : _doc(NULL),
      _root(NULL),
      _nodeFactory(new DefNodeFactory()),
      _debug(false)
{
    if ( ! XmlDocument::_Libinit )
        XmlDocument::LibInit();

    this->readMemory(xmlfrag, len);
}


XmlDocument::XmlDocument ( const std::string & filename, bool create )
    : _roottag(DEFAULT_ROOT_TAGNAME),
      _doc(NULL),
      _root(NULL),
      _nodeFactory(new DefNodeFactory()),
      _debug(false)
{
    if ( ! XmlDocument::_Libinit )
        XmlDocument::LibInit();
    
    this->initDocument(filename, create);
}


XmlDocument::~XmlDocument()
{
    this->clearDocument();
    if ( _nodeFactory )
        delete _nodeFactory;
}

//-------------------------------------------------------------//

/**  Initializes this XmlDocument object with the provided XML file.
  *  The existing doc structure is cleared if any exists.
 **/
bool
XmlDocument::initDocument ( const std::string & filename, bool create )
{
    xmlNodePtr  node;
    bool        result = false;

    if ( XmlDocument::IsReadable(filename) ) 
    {
        if ( create ) {
            _errStr = "File already exists, remove first to create";
            result  = false;
        } else {
            result = this->readFile(filename);
        }
    } 
    else if ( create ) 
    {
        _xmlfile = filename;

        if ( _doc != NULL )
            this->clearDocument();

        // create new xml document
        _doc = ::xmlNewDoc((const xmlChar*) "1.0");
        node = ::xmlNewNode(NULL, (const xmlChar*) _roottag.c_str());
        ::xmlDocSetRootElement(_doc, node);

        _root = new XmlNode(NULL, node); // pass false too??
        _root->setAttr("Version", std::string(DEFAULT_ROOT_VERSION));
        result = true;
    }

    return result;
}

//-------------------------------------------------------------//

/**  Returns a boolean indicating the current state of the 
  *  underlying xml parser. If the document initialization has 
  *  failed or has not yet occured, this returns false
 **/
bool
XmlDocument::docIsValid() const
{
    if ( NULL == _doc )
        return false;
    return true;
}

//-------------------------------------------------------------//

/**  Parses the provided string representing a well-formed xml
  *  document or document fragment.
 **/
bool
XmlDocument::readMemory ( const char * xmlblob, size_t len )
{
    bool  result = false;

    if ( _root )
        this->clearDocument();

    result = this->initParser(xmlblob, len);

    if ( result && _doc ) {
        _root = (*this->_nodeFactory)(NULL, xmlDocGetRootElement(_doc));
        _root->setDebug(this->_debug);
    } else {
        this->clearDocument();
    }

    return result;
}


/**  Initializes the internal document structure and 
  *  parses the provided filename using the
  *  libxml2 Parser
 **/
bool
XmlDocument::readFile ( const std::string & filename )
{
    bool result = false;

    if ( _root )
        this->clearDocument();

    if ( XmlDocument::IsReadable(filename) ) {
        _xmlfile = filename;
        result = this->initParser(filename);
    }

    if ( result && _doc ) {
        _root = (*this->_nodeFactory)(NULL, xmlDocGetRootElement(_doc));
        _root->setDebug(this->_debug);
    } else {
        this->clearDocument();
    }

    return result;
}

//-------------------------------------------------------------//

/**  Writes out the xml document to the provided filename */
bool
XmlDocument::writeFile ( const std::string & filename )
{
    int r = -1;

    if ( _encoding.length() == 0 ) {
        r = ::xmlSaveFormatFile(filename.c_str(), _doc, 1);
    } else {
        r = ::xmlSaveFormatFileEnc(filename.c_str(), _doc, _encoding.c_str(), 1);
    }
    
    if ( r < 0 )
        return false;

    return true;
}

//-------------------------------------------------------------//

/**  Sets a DTD file to be used for validation.
  *  Currently supported in conjunction with the 
  *  Parser API, this must be set prior to calling
  *  readFile().
 **/
void
XmlDocument::setDTDFile ( const std::string & filename )
{
    _dtdfile = filename;
}


/**  Saves the internal xml document back to the original file.
  *  This will return false if the document was initialized by
  *  memory instead of by a file.
 **/
bool
XmlDocument::saveDocument()
{
    if ( _xmlfile.length() == 0 )
        return false;
    return this->writeFile(_xmlfile);
}


/**  Clears the document object of all data structures, essentially 
  *  resetting the object to nil. */
void
XmlDocument::clearDocument()
{
    if ( _root ) {
        delete _root;
        _root = NULL;
    }
    if ( _doc ) {
        xmlFreeDoc(_doc);
        _doc = NULL;
    }

    xmlCleanupParser();
}

//-------------------------------------------------------------//

/**  Sets a new XmlNodeFactory handler, overriding and releasing any 
  *  existing or default handler.
 **/
void
XmlDocument::setNodeFactory ( XmlNodeFactory * nodeFactory )
{
    if ( this->_nodeFactory )
        delete _nodeFactory;
    _nodeFactory = nodeFactory;
    return;
}

//-------------------------------------------------------------//

/**  Returns a pointer to the document's root XmlNode
  *  object or NULL if none exists.
 **/
XmlNode*
XmlDocument::getRootNode()
{
    return _root;
}


/**  Sets a new document root node and returns the old detached
  *  root node or NULL if none exists.  The erase boolean allows 
  *  the setting of a new root node and forcing the immediate 
  *  delete of the old root node also returning NULL. 
  *  This is the default behavior unless erase is false, in 
  *  which case the detached node is returned, and it is the 
  *  callers responsibility to free the object.
 **/
XmlNode*
XmlDocument::setRootNode ( XmlNode * node, bool erase )
{
    XmlNode*    tmp = NULL;
    xmlNodePtr  n;

    if ( node == NULL || node->getNode() == NULL )
        return NULL;

    if ( _root ) 
    {
        n = _root->getNode();
        if ( n->parent )
            ::xmlUnlinkNode(n);
        tmp = _root;
    }

    n = node->getNode();

    if ( node->getParent() ) 
    {
        node->getParent()->removeNode(node);
        node->setParent(NULL);

        if ( n->parent )
            ::xmlUnlinkNode(n);
    }
        
    ::xmlDocSetRootElement(_doc, n);
    ::xmlSetTreeDoc(n, _doc);
    _root = node;

    if ( erase && tmp != NULL ) {
        delete tmp;
        tmp = NULL;
    }

    return tmp;
}


/**  Renames the root element */
void
XmlDocument::setRootTagName ( const std::string & tagname )
{
    xmlNodePtr  n;

    _roottag = tagname;

    if ( _root == NULL )
        return;

    n = _root->getNode();
    ::xmlNodeSetName(n, (const xmlChar*) _roottag.c_str());

    return;
}

//-------------------------------------------------------------//

/**  Internal function utilizing libxml2 Parser API.
  *  The parser uses the DOM model keeping all document
  *  and tree elements in memory. The XmlNode objects 
  *  will keep a reference to their respective node 
  *  pointers to provide a 'live' interface to xml tree
  *  elements.
 **/
bool
XmlDocument::initParser ( const std::string & filename )
{
    if ( filename.length() > 0 )
        _doc = ::xmlParseFile(filename.c_str());

    if ( _doc == NULL ) {
        _errStr = "Error: parse error in XML document: ";
        _errStr.append(filename);
        return false;
    }

    if ( ! this->validate() ) {
        _errStr = "Error: XML failed DTD validation";
        this->clearDocument();
        return false;  // hello _doc??
    }

    if ( _doc->encoding )
        _encoding = (const char*) _doc->encoding;

    return true;
}


/**  Initializes the internal xml doc document by parsing the 
  *  provided xml string
 **/
bool
XmlDocument::initParser ( const char * xmlblob, size_t len )
{
    _doc = ::xmlParseMemory(xmlblob, len);

    if ( _doc == NULL ) {
        _errStr = "Error in parse: xml malformed";
        return false;
    }

    if ( ! this->validate() ) {
        _errStr = "Error: xml failed DTD validation";
        return false;
    }
    
    if ( _doc->encoding )
        _encoding = (const char*) _doc->encoding;

    return true;
}

//-------------------------------------------------------------//

/**  Validates the internal document using the defined DTD. If no dtd 
  *  has been defined, the validation is not attempted and this simply 
  *  returns true.
 **/
bool
XmlDocument::validate()
{
    if ( _doc == NULL )
        return false;

    if ( _dtdfile.length() > 0 ) 
    {
        xmlDtdPtr    dtd;
        xmlValidCtxt cvp;

        dtd = ::xmlParseDTD(NULL, (const xmlChar*) _dtdfile.c_str());

        if ( ! xmlValidateDtd(&cvp, _doc, dtd) ) {
            ::xmlFreeDtd(dtd);
            return false;
        } else {
            ::xmlFreeDtd(dtd);
        }
    }

    return true;
}    

//-------------------------------------------------------------//

/**  attachNode - Adds the provided node to the provided parent 
  *  ensuring that the undlerlying document pointers are properly 
  *  relinked.
 **/
bool
XmlDocument::attachNode ( XmlNode * parent, XmlNode * node )
{
    xmlNodePtr  p, n;

    if ( parent == NULL || node == NULL )
        return false;

    p = parent->getNode();
    n = node->getNode();

    if ( p == NULL || n == NULL )
        return false;

    ::xmlUnlinkNode(n);
    ::xmlAddChild(p, n);
    ::xmlSetTreeDoc(n, _doc);
    
    node->setParent(parent);
    parent->addNode(node);

    return true;
}

//-------------------------------------------------------------//

void
XmlDocument::setDebug ( bool d )
{
    this->_debug = d;
}


const std::string &
XmlDocument::getErrorStr() const
{
    return _errStr;
}

//-------------------------------------------------------------//

/**  Determines whether the file exists and is readable. */
bool
XmlDocument::IsReadable ( const std::string & filename )
{
    std::ifstream ifn(filename.c_str());

    if ( !ifn )
        return false;

    ifn.close();
    return true;
}

//-------------------------------------------------------------//

void
XmlDocument::LibInit()
{
    LIBXML_TEST_VERSION;
    _Libinit = true;
}

//-------------------------------------------------------------//

const char*
XmlDocument::Version()
{
    return _Version;
}

//-------------------------------------------------------------//

std::string
XmlDocument::NodeToString ( XmlNode * node )
{
    xmlBufferPtr  xmlbuf;
    std::string   nodestr = "";

    if ( node == NULL )
        return nodestr;

    if ( (xmlbuf = ::xmlBufferCreate()) == NULL )
        return nodestr;


    ::xmlNodeDump(xmlbuf, _doc, node->getNode(), 0, 0);
    nodestr.assign( (const char*) ::xmlBufferContent(xmlbuf) );
    ::xmlBufferFree(xmlbuf);

    return nodestr;
}

}  // namespace


/*  _TCAXMLPLUS_XMLDOCUMENT_CPP_  */

