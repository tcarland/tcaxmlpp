
#include <cstdio>
#include <string>

#include <iostream>


#include "XmlDocument.h"
#include "XmlDocNode.h"
using namespace tcaxmlpp;

void usage()
{
    printf("Usage: nodeftest <infile.xml>\n");
    exit(0);
}


int main ( int argc, char ** argv )
{
    std::string  infile = "test.xml";

    printf("infile: %s\n", infile.c_str());

    XmlDocument    doc;
    doc.setDebug(true);
    doc.setNodeFactory(new XmlDocNodeFactory(&doc));

    if ( ! doc.initDocument(infile) ) {
        printf("Doc err: %s\n", doc.getErrorStr().c_str());
        return -1;
    }

    XmlDocNode * root = (XmlDocNode*) doc.getRootNode();
    root->printNode();

    XmlNode::iterator  nIter;
    for ( nIter = root->begin(); nIter != root->end(); ++nIter )
    {
        std::cout << "Node name: " << (*nIter)->getNodeName() << std::endl;
    }

    XmlDocNode * node = (XmlDocNode*) root->findNode("tnmsd");
    if ( node == NULL )
        return -1;
    node = (XmlDocNode*) node->findNode("server");
    if ( node == NULL )
        return -1;

    XmlDocument * docptr = node->getDocument();
    XmlDocument * dptr   = &doc;

    if ( dptr != docptr ) {
        std::cout << "Error in xmldocnode ptr" << std::endl;
        return -1;
    }

    std::string xmlblob = docptr->NodeToString(root);
    printf("doc: \n%s\n", xmlblob.c_str());


    return 0;
}






