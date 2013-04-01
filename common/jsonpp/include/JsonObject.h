/**
  * @file JsonObject.h
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
#ifndef _JSONPP_JSONOBJECT_H_
#define _JSONPP_JSONOBJECT_H_

#include <map>
#include <iterator>

#include "JsonItem.hpp"
#include "JsonException.hpp"


namespace jsonpp {


/** The JsonObject class represents the core JSON type of 
  * an associative array. The STL map container is used
  * as the underlying container.
 **/
class JsonObject : public JsonItem {

  public:

    typedef std::map<std::string, JsonItem*> JsonItems;
    typedef JsonItems::iterator              iterator;
    typedef JsonItems::const_iterator        const_iterator;
    typedef std::pair<iterator, bool>        pairI;
    typedef JsonItems::size_type             size_type;

  public:

    JsonObject();
    JsonObject ( const JsonObject & obj );

    virtual ~JsonObject() throw ();

    JsonObject&     operator=  ( const JsonObject  & obj );

    JsonItem*       operator[] ( const std::string & key ) 
                        throw ( JsonException );

    const JsonItem* operator[] ( const std::string & key ) const
                        throw ( JsonException );

    pairI           insert ( const std::string & key, JsonItem * item )
                        throw ( JsonException );

    iterator        begin() { return _items.begin(); }
    iterator        end()   { return _items.end(); }

    const_iterator  begin() const { return _items.begin(); }
    const_iterator  end()   const { return _items.end(); }

    void            erase  ( iterator at );
    void            erase  ( iterator first, iterator last );
    size_type       erase  ( const std::string & key );

    iterator        find   ( const std::string & key );
    const_iterator  find   ( const std::string & key ) const;
 
    size_t          size()  const { return _items.size(); }
    bool            empty() const { return _items.empty(); }
    void            clear();
  
    virtual std::string toString() const;

  protected:

    JsonItems      _items;
};

} // namespace

#endif // _JSONPP_JSONOBJECT_H_

