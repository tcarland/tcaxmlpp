#define _TCAXMLPP_XMLDOCNODE_CPP_


#include "XmlDocNode.h"

namespace tcaxmlpp {



XmlNode*
XmlDocNodeFactory::operator() ( XmlNode * parent, xmlNodePtr   node, bool r )
{
    return( (XmlNode*) new XmlDocNode(_doc, parent, node, r) );
}

XmlDocNode::XmlDocNode ( XmlDocument * doc, XmlNode * parent,
                         xmlNodePtr    node, bool     recurse )
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

