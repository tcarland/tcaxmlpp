/**  SqlException.hpp
  *     Exception class thrown within the library.
  *  
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
  *
  * This file is part of tcasqlpp.
  *
  * tcasqlpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcasqlpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcasqlpp.  
  * If not, see <http://www.gnu.org/licenses/>.
  *
 **/
#ifndef _INCLUDE_SQLEXCEPTION_HPP_
#define _INCLUDE_SQLEXCEPTION_HPP_

#include <string>
#include <stdexcept>


namespace tcasqlpp {


class SqlException : public std::runtime_error {

  public:

    explicit SqlException ( const std::string & err ) 
        : std::runtime_error(err) {}

    virtual ~SqlException() throw() {}

    virtual std::string toString() const 
    {
        return this->what();
    }

};

} // namespace

#endif  // _INCLUDE_SQLEXCEPTION_HPP_

