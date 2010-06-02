/**
  *   SocketOption.h
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
#ifndef _TCANETPP_SOCKETOPTION_H_
#define _TCANETPP_SOCKETOPTION_H_

#include <string>


namespace tcanetpp {


class SocketOption {
	
public:
    
    SocketOption();
    SocketOption ( int level, int optname, int optval,
                   const std::string & name = "" );
    
    virtual ~SocketOption() {}
    
    
    int         level() const;
    inline int  getOptionLevel() const { return this->level(); }

    int         id() const;
    inline int  getOptionId() const { return this->id(); }

    int         value() const;
    inline int  getOptionValue() const { return this->value(); }

    
    const std::string&   name() const;
    inline
    const std::string&   getOptionName() const { return this->name(); }
    inline
    const std::string&   toString() const { return this->name(); }


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

    static SocketOption  SetNoFragment ( int val );
    static SocketOption  SetTTL        ( int val );
    
private:

    int                 _level;
    int                 _optid;
    int             	_optval;
    std::string     	_namestr;
	
};

} // namespace

#endif  // _TCANETPP_SOCKETOPTION_H_ 

