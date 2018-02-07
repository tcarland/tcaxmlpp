/**
  * @file XmlDockNode.h
  *
  * Copyright(c) 2008-2018 Timothy Charlton Arland
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
#ifndef _TCAXMLPP_XMLDOCNODE_H_
#define _TCAXMLPP_XMLDOCNODE_H_

#include "XmlNode.h"


namespace tcaxmlpp {


/** Specialized XmlNodeFactory for creating XmlNode
  * objects that contain a reference to the parent
  * XmlDocument.
 **/
class XmlDocNodeFactory : public XmlNodeFactory {
    XmlDocument * _doc;
  public:
    explicit XmlDocNodeFactory ( XmlDocument * doc )
        : _doc(doc)
    {}

    virtual ~XmlDocNodeFactory() {}

    virtual XmlNode* operator() ( XmlNode    * parent,
                                  xmlNodePtr   node,
                                  bool         recurse );
};


/** A sepecialized XmlNode object that holds a reference
  * to its parent document.
 **/
class XmlDocNode : public XmlNode {

  public:

    XmlDocNode ( XmlDocument * doc, XmlNode * parent,
                 xmlNodePtr    node, bool     recurse );

    virtual ~XmlDocNode() {}

    XmlDocument* getXmlDocument();
    XmlDocument* getDocument();

  protected:

    XmlDocument * _doc;
};

}  // namespace

#endif  // _TCAXMLPP_XMLDOCNODE_H_
