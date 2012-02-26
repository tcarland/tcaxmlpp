/**
  * @file Exception.hpp
  *
  *  Base Exception class used and thrown by the tcanetpp library.
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
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
#ifndef _TCANETPP_EXCEPTION_HPP_
#define _TCANETPP_EXCEPTION_HPP_

extern "C" {
# include <stdio.h>
}

#include <string>
#include <stdexcept>


namespace tcanetpp {


/**  Base Exception class used and thrown by the tcanetpp library. */
class Exception : public std::runtime_error {
	
public:
    
    explicit Exception ( const std::string & errstr )
        : std::runtime_error(errstr)
    {}
    
    virtual ~Exception() throw() {}
    
    virtual void printErr() const
    {
        fprintf(stderr, "Exception: %s\n", this->what());
    }
    
    virtual std::string toString() const
    {
        return this->what();
    }
    
};

} // namespace


#endif /*_TCANETPP_EXCEPTION_HPP_*/
