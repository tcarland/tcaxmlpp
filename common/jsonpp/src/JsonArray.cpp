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

JsonArray::iterator
JsonArray::erase ( JsonArray::iterator at )
{
    return _items.erase(at);
}

// ------------------------------------------------------------------------- //

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

