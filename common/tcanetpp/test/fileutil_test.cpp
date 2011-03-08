#define _FILEUTILS_TEST_CPP_

#include <cstdlib>

#include <string>
#include <iostream>

#include "FileStat.h"
using namespace tcanetpp;


void usage()
{
    std::cout << "Usage: filetest <path/to/file>" << std::endl;
    exit(0);
}

int main ( int argc, char **argv )
{
    if ( argc < 2 )
        usage();

    std::string file = argv[1];

    FileStat f(file);

    std::cout << "path: " << f.getPathName() << std::endl;
    std::cout << "file: " << f.getFileName() << std::endl;

    return 0;
}


