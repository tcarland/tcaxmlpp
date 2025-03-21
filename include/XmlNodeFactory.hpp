/**
  * @file XmlNodeFactory.hpp
  *
  * Class for manipulating a node, its children, and their attributes.
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
#ifndef _TCAXMLPP_XMLNODEFACTORY_HPP_
#define _TCAXMLPP_XMLNODEFACTORY_HPP_

#include <libxml/tree.h>


namespace tcaxmlpp {

class XmlNode;


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

}

#endif  // _TCAXMLPP_XMLNODEFACTORY_HPP_
