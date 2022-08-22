// xmltest.cpp
//
#include <cstdio>
#include <string>

#include "XmlDocument.h"
using namespace tcaxmlpp;


int main ( int argc, char ** argv )
{
    std::string  infile;

    if ( argc > 1 )
        infile = argv[1];
    else
        exit(0);
    printf("infile: %s\n", infile.c_str());

    XmlDocument    doc;
    doc.setDebug(true);

    if ( ! doc.initDocument(infile) ) {
        printf("Doc err: %s\n", doc.getErrorStr().c_str());
        return -1;
    }

    XmlNode * root = doc.getRootNode();

    std::string xmlblob = doc.NodeToString(root);

    XmlDocument  ndoc(xmlblob.c_str(), xmlblob.length());
    printf("new doc: \n%s\n", ndoc.NodeToString(ndoc.getRootNode()).c_str());
    ndoc.writeFile("xmltestout.xml");

    // find a specific node
    //
    printf("Find A node:\n");
    XmlNode * tnode = root->findNode("tnmsd");
    XmlNode * cnode = tnode->findNode("client", "name", "server_a");
    if ( cnode ) {
        printf("  Found node: %s : %s\n", cnode->getNodeName().c_str(),
            cnode->getAttr("name").c_str());
    }
    XmlNode * dnode = tnode->findNode("client", "name");
    if ( dnode ) {
        printf("  Found node: %s : %s\n", dnode->getNodeName().c_str(),
            dnode->getAttr("name").c_str());
    } else
        printf("Failed to find client name node\n");

    XmlNode * enode = tnode->findNode("client");
    if ( enode ) {
        printf("  Found node: %s : %s\n", enode->getNodeName().c_str(),
            enode->getAttr("name").c_str());
    } else
        printf("Failed to find client node\n");
    
    XmlNodeList nodes;

    nodes = tnode->findNodes("client");
    printf("Found %lu nodes looking for <client>\n", nodes.size());

    XmlDocument newdoc;
    newdoc.setDebug(true);

    XmlNode * newnode = new XmlNode(NULL, "NewNode");
    newnode->setAttribute("name", "Testing New Node");
    newnode->setAttribute("key", "value");
    
    newdoc.setRootNode(newnode);

    XmlNode * subnode = new XmlNode(newnode, "SubNode");
    subnode->setAttribute("name", "Testing new subnode");

    xmlblob = newdoc.NodeToString(newnode);
    printf("new doc: \n%s\n", xmlblob.c_str());


    return 0;
}






