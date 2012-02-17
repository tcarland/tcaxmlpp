/**
  * @file SocketOption.h
  *
  * Copyright (c) 2008 Timothy Charlton Arland 
  * @author tca@charltontechnology.net
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
#ifndef _TCANETPP_SOCKETOPTION_H_
#define _TCANETPP_SOCKETOPTION_H_

#include <string>


namespace tcanetpp {


/**  An instance of this class represents a single low-level 
  * socket option consisting of the socket 'level' (SO_SOCKET),
  * the option name and corresponding value.
  * This is used mostly as a convenient factory class for 
  * creating a socket option for use by the tcanetpp::Socket class
  * through use of the static factory methods.
 **/
class SocketOption {
	
public:
    
    SocketOption();

    SocketOption ( int level, int optname, int optval,
                   const std::string & name = "" );
    
    virtual ~SocketOption() {}
    
    
    int         level() const;
    int         id() const;
    int         value() const;

    inline int  getOptionLevel() const { return this->level(); }
    inline int  getOptionId() const    { return this->id(); }
    inline int  getOptionValue() const { return this->value(); }

    
    const std::string&   name() const;
    inline
    const std::string&   getOptionName() const { return this->name(); }
    inline
    const std::string&   toString() const { return this->name(); }


    /*  Static Factory Methods  */
            
    static SocketOption  SetReuseAddr  ( int val );
    static SocketOption  SetLinger     ( int val );
    static SocketOption  SetKeepalive  ( int val );
    static SocketOption  SetRcvBuf     ( int val );
    static SocketOption  SetSndBuf     ( int val );
    static SocketOption  SetRcvLoWat   ( int val );
    static SocketOption  SetSndLoWat   ( int val );
    static SocketOption  SetRcvTimeout ( int val );
    static SocketOption  SetSndTimeout ( int val );
    static SocketOption  SetNoFragment ( int val );
    static SocketOption  SetTTL        ( int val );
    

protected:

    std::string     	_namestr;
    int                 _level;
    int                 _optid;
    int             	_optval;
	
};

} // namespace

#endif  // _TCANETPP_SOCKETOPTION_H_ 

