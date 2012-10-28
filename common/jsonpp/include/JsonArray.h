#pragma once
#ifndef _JSONPP_JSONARRAY_H_
#define _JSONPP_JSONARRAY_H_

#include <deque>

#include "JsonItem.hpp"
#include "JsonException.hpp"

namespace jsonpp {


/** JsonArray represents a one-dimensional array of JsonItems. */
class JsonArray : public JsonItem {
  public:

    typedef std::deque<JsonItem*>      ArrayItems;
    typedef ArrayItems::iterator       iterator;
    typedef ArrayItems::const_iterator const_iterator;
    typedef ArrayItems::size_type      size_type;

  public:

    JsonArray();
    JsonArray ( const JsonArray & ary );
    virtual ~JsonArray();

    JsonArray&      operator=  ( const JsonArray & ary );
    JsonItem*       operator[] ( size_type index );
    const JsonItem* operator[] ( size_type index ) const;

    iterator        begin() { return _items.begin(); }
    iterator        end()   { return _items.end(); }

    const_iterator  begin() const { return _items.begin(); }
    const_iterator  end()   const { return _items.end(); }

    iterator        insert ( JsonItem * item );
    iterator        insert ( JsonItem * item, iterator at );
    iterator        erase  ( iterator at );

    size_t          size()  const { return _items.size(); }
    bool            empty() const { return _items.empty(); }
    void            clear();

    JsonItem*       at ( size_type index );
    const JsonItem* at ( size_type index ) const;

    virtual std::string toString() const;

  private:

    ArrayItems     _items;
};


} // namespace

#endif // _JSONPP_JSONARRAY_H_

