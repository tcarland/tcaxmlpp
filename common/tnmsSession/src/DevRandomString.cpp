#define _TNMSSESSION_DEVRANDOMSTRING_CPP_

#include "DevRandomString.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



namespace tnmsSession {


DevRandomString::DevRandomString ( const std::string & filename )
{
    this->_source = ::fopen(filename.c_str(), "r" );
}

DevRandomString::~DevRandomString()
{
    if ( this->_source )
        ::fclose(_source);
}

void
DevRandomString::randomString ( std::string & randomString, size_t length, 
                                bool printable )
{
    return this->randomString("", randomString, length, printable);
}


void
DevRandomString::randomString ( const std::string & prefix, 
                                std::string & randomString,
                                size_t length, bool printable )
{
    if ( ! this->_source )
        return;
    
    char  *  rbuf;
    size_t   rbytes;
    
    randomString = prefix;
    rbuf         = (char*) ::malloc((length * sizeof(char)) + 1);
    rbuf[length] = 0;
    
    rbytes = ::fread(rbuf, sizeof(char), length, this->_source);
    
    //if ( rbytes != len ) throw
    
    if ( printable )
        this->makePrintable(rbuf, length);
    
    randomString += rbuf;

    return;
}

void 
DevRandomString::makePrintable ( char * buff, size_t buff_len )
{
    for ( size_t i = 0; i < buff_len; ++i )
        buff[i] = ( (unsigned char) buff[i] % ('~' - '0')) + '0';
}

}  // namespace


//  _TNMSSESSION_DEVRANDOMSTRING_CPP_
