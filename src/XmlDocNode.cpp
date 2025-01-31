/**
  * @file XmlDockNode.cpp
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
#define _TCAXMLPP_XMLDOCNODE_CPP_

#include "XmlDocNode.h"


namespace tcaxmlpp {


XmlNode*
XmlDocNodeFactory::operator() ( XmlNode * parent, xmlNodePtr node, bool r )
{
    return( (XmlNode*) new XmlDocNode(_doc, parent, node, r) );
}


/** 
 *  The XmlDocNode class provides a convenient XmlNode heirarchy
 *  that keeps a pointer to the parent XmlDocument container object
 *  for a given node. This also serves as a simple example of using
 *  the XmlNodeFactory class for creating specialized XmlNode objects.
 *
 *  Note that XmlNodeFactory only provides a factory class for
 *  customizing the root level node. Further node customization is
 *  accomplished by overriding XmlNode::recursiveWalk().
 */
XmlDocNode::XmlDocNode ( XmlDocument * doc,
                         XmlNode     * parent,
                         xmlNodePtr    node,
                         bool          recurse )
    : XmlNode(parent, node, recurse),
      _doc(doc)
{}


XmlDocument*
XmlDocNode::getDocument()
{
    if ( this->_parent == NULL )
        return this->_doc;

    XmlDocNode * p = this;

    while ( p->getParent() != NULL )
        p = (XmlDocNode*) p->getParent();

    return p->getDocument();
}

}  // namespace

//  _TCAXMLPP_XMLDOCNODE_CPP_
