#define __CIRBUFFER_TEST__

#include <cstdlib>
#include <iostream>

#include "CircularBuffer.h"
#include "StringUtils.h"

using namespace tcanetpp;


int main ( int argc, char ** argv )
{
    size_t  buffsize = DEFAULT_CIRBUFFER_SIZE;
    size_t  maxsize  = MAX_CIRBUFFER_SIZE;

    if ( argc == 2 )
        buffsize = StringUtils::fromString<size_t>(argv[1]);

    CircularBuffer * buff  = new CircularBuffer(buffsize);
    std::string      bstr  = "0123456789";
    int              count = buffsize / bstr.length();

    std::cout << " buffer capacity = " << buff->size() 
        << ", max is " << maxsize
        << ", string '" << bstr << std::endl
        << "', count is " << count << std::endl << std::endl;

    while ( buff->writeAvailable() >= bstr.length() )
        buff->write(bstr.c_str(), bstr.length());

    char * out = (char*) calloc(bstr.length(), sizeof(char));

    std::cout << " dataAvail in buffer = " << buff->readAvailable()
              << std::endl;

    for ( int i = 0; i < (count / 2); ++i ) {
        buff->read(out, bstr.length());
    }
    std::cout << " read: '" << out << "'" << std::endl;

    buffsize = buffsize - (count / 2);
    std::cout << " resizing to " << buffsize << std::endl;

    if ( ! buff->resize(buffsize) )
        std::cout << "RESIZE FAILED" << std::endl;

    std::cout << " buffer capacity = " << buff->size() << std::endl 
        << " fullDataAvail = " << buff->readAvailable()
        << " dataAvail = " << buff->readPtrAvailable() << std::endl
        << " fullSpaceAvail = " << buff->writeAvailable()
        << " spaceAvail = " << buff->writePtrAvailable()
              << std::endl << std::endl;

    int c = 0;
    while ( buff->writePtrAvailable() >= bstr.length() ) {
        buff->write(bstr.c_str(), bstr.length());
        c++;
    }
    std::cout << " write count = " << c << std::endl << std::endl;

    std::cout << " buffer capacity = " << buff->size() << std::endl 
              << " fullDataAvail = "   << buff->readAvailable()
              << " dataAvail = "       << buff->readPtrAvailable() << std::endl
              << " fullSpaceAvail = "  << buff->writeAvailable()
              << " spaceAvail = "      << buff->writePtrAvailable()
              << std::endl << std::endl;

    buffsize = buff->size() + bstr.length();
    std::cout << " resizing to " << buffsize << std::endl;
    
    if ( ! buff->resize(buffsize) )
        std::cout << " RESIZE FAILED " << std::endl;

    std::cout << " buffer capacity = " << buff->size() << std::endl 
              << " fullDataAvail = "   << buff->readAvailable()
              << " dataAvail = "       << buff->readPtrAvailable() << std::endl
              << " fullSpaceAvail = "  << buff->writeAvailable()
              << " spaceAvail = "      << buff->writePtrAvailable()
              << std::endl << std::endl;

    ::free(out);
    delete buff;

    return 0;
}


