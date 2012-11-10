#pragma once
#ifndef _JSONPP_JSONOBJECT_H_
#define _JSONPP_JSONOBJECT_H_

#include <map>
#include <iterator>

#include "JsonItem.hpp"
#include "JsonException.hpp"


namespace jsonpp {


/** The JsonObject class represents the core JSON type of 
  * an associative array. The STL map container is used
  * to store the items.
 **/
class JsonObject : public JsonItem {

  public:

    typedef std::map<std::string, JsonItem*> JsonItems;
    typedef JsonItems::iterator              iterator;
    typedef JsonItems::const_iterator        const_iterator;
    typedef std::pair<iterator, bool>        pairI;
    typedef JsonItems::size_type             size_type;

  public:

    JsonObject() : JsonItem(JSON_OBJECT) {}
    JsonObject ( const JsonObject & obj );

    virtual ~JsonObject() throw ();

    JsonObject&     operator=  ( const JsonObject  & obj );

    JsonItem*       operator[] ( const std::string & key ) 
                        throw ( JsonException );

    const JsonItem* operator[] ( const std::string & key ) const
                        throw ( JsonException );

    iterator        begin() { return _items.begin(); }
    iterator        end()   { return _items.end(); }

    const_iterator  begin() const { return _items.begin(); }
    const_iterator  end()   const { return _items.end(); }

    pairI           insert ( const std::string & key, JsonItem * item )
                        throw ( JsonException );

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

