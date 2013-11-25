/**
  * @file JsonItem.hpp
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
#ifndef _JSONPP_JSONITEM_HPP_
#define _JSONPP_JSONITEM_HPP_

#include <string>


namespace jsonpp {


#define TOKEN_ARRAY_BEGIN      '['
#define TOKEN_ARRAY_END        ']'
#define TOKEN_OBJECT_BEGIN     '{'
#define TOKEN_OBJECT_END       '}'
#define TOKEN_NAME_SEPARATOR   ':'
#define TOKEN_VALUE_SEPARATOR  ','
#define TOKEN_STRING_SEPARATOR '"'
#define TOKEN_WS               ' '


/**  The JSON type used to identify JsonItems */
// TODO: Use JSON_LITERAL instead of bool hack and null
typedef enum JsonValueType {
    JSON_INVALID,
    JSON_ITEM,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_NUMBER,
    JSON_STRING,
    JSON_BOOL_TRUE,
    JSON_BOOL_FALSE,
    JSON_NULL
} json_t;


/**  JsonItem is the base class of all JSON types.  */
class JsonItem {

  public:

    JsonItem ( json_t  t = JSON_ITEM ) : _type(t) {}
    virtual ~JsonItem() {}

    json_t   getType()      const { return _type; }
    json_t   getValueType() const { return this->getType(); }

    virtual std::string toString() const
    {
        if ( _type == JSON_NULL )
            return std::string("null");
        return std::string("UNKNOWN");
    }

  protected:

    json_t   _type;
};


} // namespace

#endif // _JSONPP_JSONITEM_HPP_

