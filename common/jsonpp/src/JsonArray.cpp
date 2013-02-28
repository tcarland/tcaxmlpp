/**
  * @file JsonArray.cpp
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
#define _JSONPP_JSONARRAY_CPP_

#include "JsonArray.h"
#include "JSON.h"


namespace jsonpp {


// ------------------------------------------------------------------------- //


JsonArray::JsonArray()
    : JsonItem(JSON_ARRAY)
{}

JsonArray::JsonArray ( const JsonArray & ary )
    : JsonItem(JSON_ARRAY)
{
    *this = ary;
}

JsonArray::~JsonArray()
{
    this->clear();
}

// ------------------------------------------------------------------------- //

/** Assignment operator for a JsonArray. */
JsonArray&
JsonArray::operator= ( const JsonArray & ary )
{
    if ( this == &ary )
        return *this;

    this->_items.clear();
    this->_type  = ary._type;

    JsonArray::const_iterator  jIter;
    for ( jIter = ary.begin(); jIter != ary.end(); ++jIter )
    {
        json_t  t = (*jIter)->getType();
        
        switch ( t ) {
            case JSON_OBJECT:
                this->insert(new JsonObject(*(JsonObject*)*jIter));
                break;
            case JSON_ARRAY:
                this->insert(new JsonArray(*(JsonArray*)*jIter));
                break;
            case JSON_NUMBER:
                this->insert(new JsonNumber(*(JsonNumber*)*jIter));
                break;
            case JSON_STRING:
                this->insert(new JsonString(*(JsonString*)*jIter));
                break;
            case JSON_BOOL_TRUE:
            case JSON_BOOL_FALSE:
                this->insert(new JsonBoolean(*(JsonBoolean*)*jIter));
                break;
            case JSON_NULL:
                this->insert(new JsonItem(JSON_NULL));
                break;
            case JSON_ITEM:
            case JSON_INVALID:
            default:
                break;
        }
    }

    return *this;
}

// ------------------------------------------------------------------------- //

JsonItem*
JsonArray::operator[] ( JsonArray::size_type index )
{
    return _items[index];
}

const JsonItem*
JsonArray::operator[] ( JsonArray::size_type index ) const
{
    return _items[index];
}

// ------------------------------------------------------------------------- //

/** Inserts the provided JsonItem into the array. */
JsonArray::iterator
JsonArray::insert ( JsonItem * item )
{
    return _items.insert(_items.end(), item);
}

JsonArray::iterator
JsonArray::insert ( JsonItem * item, JsonArray::iterator at )
{
    return _items.insert(at, item);
}

// ------------------------------------------------------------------------- //

/** Erases the JsonItem at the given iterator position */
JsonArray::iterator
JsonArray::erase ( JsonArray::iterator at )
{
    return _items.erase(at);
}

// ------------------------------------------------------------------------- //

/** Returns the JsonItem at the given index location */
JsonItem*
JsonArray::at ( JsonArray::size_type index )
{
    return _items.at(index);
}

const JsonItem*
JsonArray::at ( JsonArray::size_type index ) const
{
    return _items.at(index);
}

// ------------------------------------------------------------------------- //

/**  Removes all elements from the JsonArray */
void
JsonArray::clear()
{
    JsonArray::iterator jIter;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter )
    {
        if ( (*jIter) )
            delete(*jIter);
    }

    return _items.clear();
}

// ------------------------------------------------------------------------- //

/** Converts the JsonArray to a proper JSON formatted string */
std::string
JsonArray::toString() const
{
    JsonArray::const_iterator jIter;
    std::stringstream  jstr;
    size_t i = 1;

    jstr << TOKEN_ARRAY_BEGIN << TOKEN_WS;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter, ++i ) {
        const JsonItem * item = *jIter;
        jstr << JSON::ToString(item);
        if ( i  < this->size() )
            jstr << TOKEN_VALUE_SEPARATOR << TOKEN_WS;
    }

    jstr << TOKEN_WS << TOKEN_ARRAY_END;

    return jstr.str();
}

// ------------------------------------------------------------------------- //


} // namespace

// _JSONPP_JSONARRAY_CPP_

