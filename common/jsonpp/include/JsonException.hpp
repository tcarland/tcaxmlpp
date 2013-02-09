/**
  * @file JsonException.hpp
  *
  * Copyright (c) 2012,2013 Timothy Charlton Arland
  * @author  tcarland@gmail.com
  *
  * @section LICENSE
  *
  * This file is part of jsonpp.
  *
  * jsonpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as
  * published by the Free Software Foundation, either version 3 of
  * the License, or (at your option) any later version.
  *
  * jsonpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with jsonpp.
  * If not, see <http://www.gnu.org/licenses/>.
**/
#ifndef _JSONPP_JSONEXCEPTION_HPP_
#define _JSONPP_JSONEXCEPTION_HPP_

#include <string>
#include <stdexcept>


namespace jsonpp {


/**  Exception class used by various jsonpp methods */
class JsonException : public std::runtime_error {

  public:

    explicit JsonException ( const std::string & errstr )
        : std::runtime_error(errstr)
    {}

    virtual ~JsonException() throw() {}

    virtual inline std::string toString() const
    {
        return this->what();
    }
};

} // namespace

#endif // _JSONPP_JSONEXCEPTION_HPP_

