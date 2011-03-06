#define _TCAXMLPP_XMLDOCNODE_CPP_


#include "XmlDocNode.h"

namespace tcaxmlpp {


XmlNode*
XmlDocNodeFactory::operator() ( XmlNode * parent, xmlNodePtr node, bool r )
{
    return( (XmlNode*) new XmlDocNode(_doc, parent, node, r) );
}



/**  XmlDocNode
 *    The XmlDocNode class provides a convenient XmlNode heirarchy
 *  that keeps a pointer to the parent XmlDocument container object
 *  for a given node. This also serves as a simple example of using
 *  the XmlNodeFactory class for creating specialized XmlNode objects.
 *
 *    Note that XmlNodeFactory only provides a factory class for
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

