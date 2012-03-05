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
