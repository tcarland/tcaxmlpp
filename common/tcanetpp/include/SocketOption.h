#ifndef SOCKETOPTION_H_
#define SOCKETOPTION_H_


namespace tcanetpp {


class SocketOption {
	
public:
	
	SocketOption();
	SocketOption ( int optname, int optval );
	
	~SocketOption();
	
	
	int  getOptionName();
	int  getOptionValue();
	
	
	/* Factory Methods */
		
	static SocketOption  SetReuseAddr  ( int val );
	static SocketOption  SetLinger     ( int val );
	static SocketOption  SetKeepalive  ( int val );
	static SocketOption  SetRcvBuf     ( int val );
	static SocketOption  SetSndBuf     ( int val );
	static SocketOption  SetRcvLoWat   ( int val );
	static SocketOption  SetSndLoWat   ( int val );
	static SocketOption  SetRcvTimeout ( int val );
	static SocketOption  SetSndTimeout ( int val );
	
private:
	
	int   _optname;
	int   _optval;
	
};

} // namespace

#endif /*SOCKETOPTION_H_*/
