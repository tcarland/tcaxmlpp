/**
  * @file SocketOption.cpp
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
#define _TCANETPP_SOCKETOPTION_CPP_


#include "SocketOption.h"
#include "Socket.h"


namespace tcanetpp {


	
SocketOption::SocketOption()	
    : _level(0),
      _optid(0),
      _optval(0)
{}

SocketOption::SocketOption ( int level, int optid, int optval,
                             const std::string & name )
    : _namestr(name),
      _level(level),
      _optid(optid),
      _optval(optval)
{}


int
SocketOption::level() const
{
    return _level;
}


int
SocketOption::id() const
{
    return _optid;
}


int
SocketOption::value() const
{
    return _optval;
}


const std::string&
SocketOption::name() const
{
    return _namestr;
}


/* Static factory methods */


SocketOption
SocketOption::SetReuseAddr ( int val )
{
    return( SocketOption(SOL_SOCKET, SO_REUSEADDR, val, "SO_REUSEADDR") );
}

SocketOption
SocketOption::SetLinger ( int val )
{
    return( SocketOption(SOL_SOCKET, SO_LINGER, val, "SO_LINGER") );
}


SocketOption
SocketOption::SetKeepalive ( int val )
{
    return( SocketOption(SOL_SOCKET, SO_KEEPALIVE, val, "SO_KEEPALIVE") );
}

SocketOption
SocketOption::SetRcvBuf ( int val )
{
    return( SocketOption(SOL_SOCKET, SO_RCVBUF, val, "SO_RCVBUF") );
}

SocketOption
SocketOption::SetSndBuf ( int val )
{
    return( SocketOption(SOL_SOCKET, SO_SNDBUF, val, "SO_SNDBUF") );
}

SocketOption
SocketOption::SetRcvLoWat ( int val )
{
    return ( SocketOption(SOL_SOCKET, SO_RCVLOWAT, val, "SO_RCVLOWAT") );
}

SocketOption
SocketOption::SetSndLoWat ( int val )
{
    return ( SocketOption(SOL_SOCKET, SO_SNDLOWAT, val, "SO_SNDLOWAT") );
}

SocketOption
SocketOption::SetRcvTimeout ( int val )
{
    return ( SocketOption(SOL_SOCKET, SO_RCVTIMEO, val, "SO_RCVTIMEO") );
}

SocketOption
SocketOption::SetSndTimeout ( int val )
{
    return ( SocketOption(SOL_SOCKET, SO_SNDTIMEO, val, "SO_SNDTIMEO") );
}


SocketOption
SocketOption::SetNoFragment ( int val )
{
# ifdef WIN32
    return ( SocketOption(IPPROTO_IP, IP_DONTFRAGMENT, val, "IP_DONTFRAGMENT") );
# elif BSD
    return ( SocketOption(IPPROTO_IP, IP_DONTFRAG, val, "IP_DONTFRAG") );
# elif __sparc
    return ( SocketOption(IPPROTO_IP, IP_DONTFRAG, val, "IP_DONTFRAG") );
# else
    //return ( SocketOption(IPPROTO_IP, IP_MTU_DISCOVER, val, "IP_MTU_DISCOVER") );
    return ( SocketOption() );
# endif
}


SocketOption
SocketOption::SetTTL ( int val )
{
    return ( SocketOption(IPPROTO_IP, IP_TTL, val, "IP_TTL") );
}


}  // namespace

//  _TCANETPP_SOCKETOPTION_CPP_

