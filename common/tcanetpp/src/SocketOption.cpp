/**
  *  SocketOption.cpp
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
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
#define _TCANETPP_SOCKETOPTION_CPP_


#include "SocketOption.h"
#include "Socket.h"


namespace tcanetpp {


	
SocketOption::SocketOption()	
    : _optname(0), 
      _optval(0)
{}

SocketOption::SocketOption ( int optname, int optval, 
                             const std::string & name )
    : _optname(optname), 
      _optval(optval),
      _namestr(name)
{}


SocketOption::~SocketOption()
{}


int
SocketOption::getOptionName()
{
    return _optname;
}


std::string
SocketOption::getOptionNameString()
{
    return _namestr;
}


int
SocketOption::getOptionValue()
{
    return _optval;
}

/* Static factory methods */

SocketOption
SocketOption::SetReuseAddr ( int val )
{
    return( SocketOption(SO_REUSEADDR, val, "SO_REUSEADDR") );
}

SocketOption
SocketOption::SetLinger ( int val )
{
    return( SocketOption(SO_LINGER, val, "SO_LINGER") );
}


SocketOption
SocketOption::SetKeepalive ( int val )
{
    return( SocketOption(SO_KEEPALIVE, val, "SO_KEEPALIVE") );
}


SocketOption
SocketOption::SetRcvBuf ( int val )
{
    return( SocketOption(SO_RCVBUF, val, "SO_RCVBUF") );
}

SocketOption
SocketOption::SetSndBuf ( int val )
{
    return( SocketOption(SO_SNDBUF, val, "SO_SNDBUF") );
}

SocketOption
SocketOption::SetRcvLoWat ( int val )
{
    return ( SocketOption(SO_RCVLOWAT, val, "SO_RCVLOWAT") );
}

SocketOption
SocketOption::SetSndLoWat ( int val )
{
    return ( SocketOption(SO_SNDLOWAT, val, "SO_SNDLOWAT") );
}

SocketOption
SocketOption::SetRcvTimeout ( int val )
{
    return ( SocketOption(SO_RCVTIMEO, val, "SO_RCVTIMEO") );
}

SocketOption
SocketOption::SetSndTimeout ( int val )
{
    return ( SocketOption(SO_SNDTIMEO, val, "SO_SNDTIMEO") );
}

}  // namespace

//  _TCANETPP_SOCKETOPTION_CPP_

