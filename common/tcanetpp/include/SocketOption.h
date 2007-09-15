#ifndef _TCANETPP_SOCKETOPTION_H_
#define _TCANETPP_SOCKETOPTION_H_

#include <string>


namespace tcanetpp {


class SocketOption {
	
public:
    
    SocketOption();
    SocketOption ( int optname, int optval, const std::string & name = "" );
    
    ~SocketOption();
    
    
    int          getOptionName();
    std::string  getOptionNameString();
    int          getOptionValue();
   
    std::string  toString();
    

    /*  static Factory Methods  */
            
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
	
    int             _optname;
    int             _optval;
    std::string     _namestr;
	
};

} // namespace

#endif  // _TCANETPP_SOCKETOPTION_H_ 

