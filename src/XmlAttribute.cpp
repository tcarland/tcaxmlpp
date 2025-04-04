/**
  * @file XmlAttribute.cpp
  *
  * Copyright(c) 2008-2025 Timothy Charlton Arland <tcarland@gmail.com>
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
#define _TCAXMLPP_XMLATTRIBUTE_CPP_

#include "XmlAttribute.h"
#include "XmlNode.h"


namespace tcaxmlpp {


XmlAttribute::XmlAttribute ( XmlNode * parent, xmlAttrPtr node )
    : _parent(parent),
      _node(node),
      _key(),
      _val()
{
    if ( _node )
    {
        _key = (char*) node->name;
        if ( _node->children )
            _val = (char*) node->children->content;
    }
}

XmlAttribute::XmlAttribute ( XmlNode * parent,
                             const std::string & key,
                             const std::string & val )
   : _parent(parent),
     _node(NULL),
     _key(key)
{
    this->setValue(val);
}


XmlAttribute::~XmlAttribute() {}


void
XmlAttribute::setValue ( const std::string & val )
{
    xmlNodePtr  node = _parent->getNode();

    this->_val = val;

    if ( node == NULL )
        return;

    this->_node = xmlSetProp(node, (const xmlChar*) _key.c_str(),
        (const xmlChar*) val.c_str());

    return;
}

}  // namespace

//  _TCAXMLPP_XMLATTRIBUTE_CPP_
