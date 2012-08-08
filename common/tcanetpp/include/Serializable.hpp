/**
  * @file Serializable.hpp
  *
  * Copyright (c) 2008,2009 Timothy Charlton Arland
  * @author tcarland@gmail.com
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
#ifndef _TCANETPP_SERIALIZABLE_HPP_
#define _TCANETPP_SERIALIZABLE_HPP_

#include "tcanetpp_types.h"


namespace tcanetpp {


/**  An interface that can be used to serialize objects and/or messages */
class Serializable {

public:

    virtual ~Serializable() {}

    virtual ssize_t  serialize   ( char * buffer, size_t buffer_len ) const = 0;
    virtual ssize_t  deserialize ( const char * buffer, size_t buffer_len ) = 0;

    virtual size_t   size() const = 0;
    virtual uint16_t message_type() const = 0;

};


} // namespace

#endif  //  _TCANETPP_SERIALIZABLE_HPP_

