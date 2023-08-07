#include "XmlDocument.h"
using namespace tcaxmlpp;

void usage() 
{
    printf("Usage: xmldump <xmlfilename>\n");
    exit(0);
}

int main ( int argc, char **argv )
{
    if ( argc == 0 )
        usage();

    std::string file = argv[1];

    XmlDocument doc(file);

    XmlNode * root = doc.getRootNode();

    if ( root == NULL ) {
        printf("Failed to get root node\n");
        return -1;
    }
    root->printNode(true);

    std::string xml = doc.NodeToString(root);

    printf("\n\n%s\n", xml.c_str());
    
    return 0;
}


