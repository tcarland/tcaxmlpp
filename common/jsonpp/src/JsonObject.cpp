#define _JSONPP_JSONOBJECT_CPP_


#include "JsonObject.h"
#include "JSON.h"


namespace jsonpp {


// ------------------------------------------------------------------------- //

JsonObject::~JsonObject() throw ()
{
    this->clear();
}

JsonObject::JsonObject ( const JsonObject & obj )
    : JsonItem(JSON_OBJECT)
{
    *this = obj;
}

// ------------------------------------------------------------------------- //

JsonObject&
JsonObject::operator= ( const JsonObject & obj )
{
    if ( this == &obj )
        return *this;

    this->_items.clear();
    this->_type  = obj._type;

    JsonObject::const_iterator jIter;
    for ( jIter = obj.begin(); jIter != obj.end(); ++jIter )
    {
        json_t  t = jIter->second->getType();

        switch ( t ) {
            case JSON_OBJECT:
                this->insert(jIter->first, new JsonObject(*((JsonObject*)jIter->second)));
                break;
            case JSON_ARRAY:
                this->insert(jIter->first, new JsonArray(*((JsonArray*)jIter->second)));
                break;
            case JSON_NUMBER:
                this->insert(jIter->first, new JsonNumber(*((JsonNumber*)jIter->second)));
                break;
            case JSON_STRING:
                this->insert(jIter->first, new JsonString(*((JsonString*)jIter->second)));
                break;
            case JSON_BOOL_TRUE:
            case JSON_BOOL_FALSE:
                this->insert(jIter->first, new JsonBoolean(*((JsonBoolean*)jIter->second)));
                break;
            case JSON_NULL:
                this->insert(jIter->first, new JsonItem(JSON_NULL));
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
JsonObject::operator[] ( const std::string & key ) throw ( JsonException )
{
    iterator iter;
    
    if ( (iter = this->find(key)) == _items.end() )
        throw ( JsonException("Error, object key not found") );
    return iter->second;
}

const JsonItem*
JsonObject::operator[] ( const std::string & key ) const throw ( JsonException )
{
    const_iterator iter = this->find(key);
    if ( iter == _items.end() )
        throw ( JsonException("Error, object key not found") );
    return iter->second;
}

// ------------------------------------------------------------------------- //

JsonObject::pairI
JsonObject::insert ( const std::string & key, JsonItem * item ) throw ( JsonException )
{
    JsonObject::iterator iter;

    if ( (iter = this->find(key)) != _items.end() )
        throw ( JsonException("Item already exists") );
    
    return _items.insert(JsonItems::value_type(key, item));
}

// ------------------------------------------------------------------------- //

void
JsonObject::erase ( JsonObject::iterator at )
{
    if ( at == _items.end() )
        return;
    if ( at->second )
        delete at->second;
    return _items.erase(at);
}

void
JsonObject::erase ( JsonObject::iterator first, JsonObject::iterator last )
{
    JsonObject::iterator iter;
    for ( iter = first; iter != last; ++iter ) {
        if ( iter->second )
            delete iter->second;
    }
    return _items.erase(first, last);
}

JsonObject::size_type
JsonObject::erase ( const std::string & key )
{
    return _items.erase(key);
}

// ------------------------------------------------------------------------- //

JsonObject::iterator
JsonObject::find ( const std::string & key )
{
    return _items.find(key);
}

JsonObject::const_iterator
JsonObject::find ( const std::string & key ) const
{
    return _items.find(key);
}

// ------------------------------------------------------------------------- //

void
JsonObject::clear()
{
    JsonObject::iterator  jIter;
    for ( jIter = _items.begin(); jIter != _items.end(); ++jIter )
    {
        if ( jIter->second )
            delete jIter->second;
    }
    return _items.clear();
}

// ------------------------------------------------------------------------- //

std::string
JsonObject::toString() const
{
    JsonObject::const_iterator jIter;
    std::stringstream jstr;
    size_t i = 1;

    jstr << TOKEN_OBJECT_BEGIN << TOKEN_WS;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter, ++i )
    {
        const std::string & key  = jIter->first;
        const JsonItem    * item = jIter->second;

        jstr << TOKEN_STRING_SEPARATOR << key << TOKEN_STRING_SEPARATOR
             << TOKEN_WS << TOKEN_NAME_SEPARATOR << TOKEN_WS
             << JSON::ToString(item);
        if ( i < this->size() )
            jstr << TOKEN_VALUE_SEPARATOR << TOKEN_WS;
    }

    jstr << TOKEN_WS << TOKEN_OBJECT_END;

    return jstr.str();
}


// ------------------------------------------------------------------------- //


} // namespace

// _JSONPP_JSONOBJECT_CPP_

