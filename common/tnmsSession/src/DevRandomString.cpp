#define _TNMSSESSION_RANDOMSTRINGDEVICE_CPP_

#include "RandomStringDevice.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



namespace tnmsSession {


RandomStringDevice::RandomStringDevice ( const std::string & filename )
{
    this->_source = ::fopen(filename.c_str(), "r" );
}


RandomStringDevice::~RandomStringDevice()
{
    if ( this->_source )
        ::fclose(_source);
}


void
RandomStringDevice::randomString ( std::string & randomString, 
                                   size_t        length, 
                                   bool          printable )
{
    return this->randomString("", randomString, length, printable);
}


void
RandomStringDevice::randomString ( const std::string & prefix, 
                                   std::string       & randomString,
                                   size_t              length, 
                                   bool                printable )
{
    if ( ! this->_source )
        return;
    
    char  *  rbuf;
    size_t   rbytes;
    
    randomString = prefix;
    rbuf         = (char*) ::malloc((length * sizeof(char)) + 1);
    rbuf[length] = 0;
    
    rbytes = ::fread(rbuf, sizeof(char), length, this->_source);
    
    //if ( rbytes != len ) throw a tissy
    
    if ( printable )
        this->makePrintable(rbuf, length);
    
    randomString += rbuf;

    return;
}

void 
RandomStringDevice::makePrintable ( char * buff, size_t buff_len )
{
    for ( size_t i = 0; i < buff_len; ++i )
        buff[i] = ( (unsigned char) buff[i] % ('~' - '0')) + '0';
}


}  // namespace

//  _TNMSSESSION_RANDOMSTRINGDEVICE_CPP_

