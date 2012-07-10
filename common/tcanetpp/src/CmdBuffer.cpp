/**
  * @file CmdBuffer.cpp 
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author  tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#define _TCANETPP_CMDBUFFER_CPP_

#include "CmdBuffer.h"


namespace tcanetpp {


const char*
CmdBuffer::EOL = "\n";


CmdBuffer::CmdBufferException::CmdBufferException ( const std::string & errstr )
    : Exception(errstr)
{}



CmdBuffer::CmdBuffer ( size_t bufsize ) throw ( CmdBufferException )
    : _cmdbuf(NULL),
      _file(NULL),
      _bufsize(bufsize),
      _eol(*CmdBuffer::EOL),
      _init(false)
{
    if ( bufsize < MINIMUM_CMDBUFFER_SIZE || bufsize > MAXIMUM_CMDBUFFER_SIZE )
        throw ( CmdBufferException("CmdBuffer error, buffersize out of bounds") );
}


CmdBuffer::CmdBuffer ( const std::string & cmd, size_t bufsize ) throw ( CmdBufferException )
    : _cmdbuf(NULL),
      _file(NULL),
      _bufsize(bufsize),
      _cmd(cmd),
      _eol(*CmdBuffer::EOL),
      _init(false)
{
    if ( bufsize < MINIMUM_CMDBUFFER_SIZE || bufsize > MAXIMUM_CMDBUFFER_SIZE )
        throw ( CmdBufferException("CmdBuffer error, buffersize out of bounds") );
    if ( ! this->Open(cmd) )
        throw ( CmdBufferException(this->_errstr) );
}


CmdBuffer::~CmdBuffer()
{
    if ( _init )
        this->Close();
    if ( _cmdbuf )
        delete _cmdbuf;
}


bool
CmdBuffer::Open ( const std::string & cmd )
{
    FILE * f = NULL;

    if ( _init )
        this->Close();

    if ( (f = ::popen(cmd.c_str(), "r")) == NULL ) {
        _errstr = "CmdBuffer::Open() error in popen, failed to open pipe stream";
        return false;
    }

    _cmdbuf = new StreamBuffer(f, std::ios_base::in, _bufsize);

    if ( ! _cmdbuf->is_open() ) {
        _errstr = "CmdBuffer::Open() error in is_open()";
        delete _cmdbuf;
        delete f;
    } else {
        _init = true;
        _file = f;
    }

    return _init;
}


void
CmdBuffer::Close()
{
    if ( _init && _cmdbuf ) {
        _cmdbuf->close();
        ::pclose(_file);
        delete _cmdbuf;
        _cmdbuf = NULL;
    }

    _init = false;

    return;
}


bool
CmdBuffer::isOpen() const
{
    if ( _init )
        return _cmdbuf->is_open();

    return false;
}


bool
CmdBuffer::haveData() const
{
    if ( ! _init )
        return false;

    if ( _cmdbuf->sgetc() == EOF )
        return false;

    return true;
}


std::string
CmdBuffer::getLine()
{
    char        ch;
    std::string line = "";

    while ( _cmdbuf->sgetc() != EOF && (ch = _cmdbuf->sbumpc()) != _eol )
        line += ch;

    return line;
}


void
CmdBuffer::getAllLines ( StringBuffer & lines )
{
    std::string line = "";
    
    //lines = std::vector<std::string>();
    lines.clear();

    if ( ! _init )
        return;

    while ( this->haveData() ) 
    {
        line = this->getLine();
        lines.push_back(line);
    }

    return;
}


bool
CmdBuffer::putLine ( const std::string & line )
{
    std::streamsize  n;

    n  = _cmdbuf->sputn(line.c_str(), line.length());

    if ( n != (std::streamsize) line.length() )
        return true;

    return true;
}

std::string
CmdBuffer::getCommand() const
{
    return _cmd;
}

std::string
CmdBuffer::getErrorStr() const
{
    return _errstr;
}

char
CmdBuffer::getEOL() const
{
    return _eol;
}

void
CmdBuffer::setEOL ( char eol )
{
    _eol = eol;
}


} // namespace

//  _TCANETPP_CMDBUFFER_CPP_ 

