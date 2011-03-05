/**
  * @file XmlAttribute.h
  *
  *   This class manages an xml attribute which is generally attached 
  * to a parent node.
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
#ifndef _TCAXMLPLUS_XMLATTRIBUTE_H_
#define _TCAXMLPLUS_XMLATTRIBUTE_H_

#include "XmlDocument.h"


namespace tcaxmlpp {


class XmlNode;


/**    An XmlAttribute object instance represents a single key/value
  * attribute pair. 
  *    This class provides an interface to XmlNode for performing 
  * 'live' xml attribute node updates if applicable. 
  * XmlAttribute objects are both created and destroyed
  * by XmlNode which should be where most interaction occurs.
 **/
class XmlAttribute {

  public:

    XmlAttribute ( XmlNode * parent, xmlAttrPtr node );

    XmlAttribute ( XmlNode * parent, 
	           const std::string & key, 
	           const std::string & val );

    virtual ~XmlAttribute();

    xmlAttrPtr          getNode()        { return _node; }
    const std::string&  getKey() const   { return _key; }
    const std::string&  getValue() const { return _val; }

    virtual void        setValue ( const std::string & val );


  protected:

    XmlNode*            _parent;
    xmlAttrPtr          _node;
    std::string         _key;
    std::string         _val;

};


}  // namespace


#endif  // _TCAXMLPLUS_XMLATTRIBUTE_H_ 

