/** SocketOption
  *
  *  
  *  @Author   tcarland@gmail.com
  *  @Version  0.1
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

