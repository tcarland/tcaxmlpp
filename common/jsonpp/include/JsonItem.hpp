#pragma once
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
// TODO: Use JSON_LITERAL instead of bool and null
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

