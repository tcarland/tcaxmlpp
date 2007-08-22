/** SocketOption
  *
  *  
  *  @Author   tcarland@gmail.com
  *  @Version  0.1
 **/
#define _SOURCE_SOCKETOPTION_CPP_


#include "SocketOption.h"
#include "Socket.h"


namespace tcanetpp {


	
SocketOption::SocketOption()	
	: _optname(0), 
	  _optval(0)
{}
	
SocketOption::SocketOption ( int optname, int optval )
	: _optname(optname), 
	  _optval(optval)
{}
	
SocketOption::~SocketOption()
{}

int
SocketOption::getOptionName()
{
	return _optname;
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
	return( SocketOption(SO_REUSEADDR, val) );
}

SocketOption
SocketOption::SetLinger ( int val )
{
	return( SocketOption(SO_LINGER, val) );
}

SocketOption
SocketOption::SetKeepalive ( int val )
{
	return( SocketOption(SO_KEEPALIVE, val) );
}

SocketOption
SocketOption::SetRcvBuf ( int val )
{
	return( SocketOption(SO_RCVBUF, val) );
}

SocketOption
SocketOption::SetSndBuf ( int val )
{
	return( SocketOption(SO_SNDBUF, val) );
}

SocketOption
SocketOption::SetRcvLoWat ( int val )
{
	return ( SocketOption(SO_RCVLOWAT, val) );
}

SocketOption
SocketOption::SetSndLoWat ( int val )
{
	return ( SocketOption(SO_SNDLOWAT, val) );
}

SocketOption
SocketOption::SetRcvTimeout ( int val )
{
	return ( SocketOption(SO_RCVTIMEO, val) );
}

SocketOption
SocketOption::SetSndTimeout ( int val )
{
	return ( SocketOption(SO_SNDTIMEO, val) );
}

}  // namespace

/*  _SOURCE_SOCKETOPTION_CPP_  */
