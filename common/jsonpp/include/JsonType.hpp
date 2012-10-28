#pragma once
#ifndef _JSONPP_JSONTYPE_HPP_
#define _JSONPP_JSONTYPE_HPP_

#include <sstream>

#include "JsonItem.hpp"


namespace jsonpp {


template <typename T>
class JsonType : public JsonItem {
  public:

    JsonType ( const T & val = T(), json_t  t = JSON_ITEM )
        : JsonItem(t),
          _value(val)
    {}

    virtual ~JsonType() {}


    JsonType<T>& operator=  ( const JsonType<T> & val )
    {
        if ( this != &val ) {
            this->_value = val._value;
            this->_type  = val._type;
        }
        return *this;
    }

    bool operator== ( const JsonType<T> & val ) const
    {
        return(_value == val._value);
    }

    operator T&() { return this->value(); }
    operator const T&() const { return this->value(); }
    
    T& value() { return _value; }
    const T& value() const { return _value; }


    virtual std::string toString() const
    {
        std::stringstream jstr;
        bool s = false;

        if ( this->getType() == JSON_STRING )
            s = true;

        if ( s )
            jstr << TOKEN_STRING_SEPARATOR;
        if ( this->getType() == JSON_NULL )
            jstr << "null";
        else if ( this->getType() == JSON_BOOL_TRUE )
            jstr << "true";
        else if ( this->getType() == JSON_BOOL_FALSE )
            jstr << "false";
        else
            jstr << _value;
        if ( s )
            jstr << TOKEN_STRING_SEPARATOR;

        return jstr.str();
    }

  private:

    T    _value;
};

typedef JsonType<double>      JsonNumber;
typedef JsonType<bool>        JsonBoolean;
typedef JsonType<std::string> JsonString;


} // namespace

#endif // _JSONPP_JSONTYPE_HPP_

