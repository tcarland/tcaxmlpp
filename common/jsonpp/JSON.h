/** @file JSON.h
  *
  * Copyright (c) 2012 Timothy Charlton Arland
  * @author tcarland@gmail.com
  * 
  * @section LICENSE
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
#ifndef _JSONPP_JSON_H_
#define _JSONPP_JSON_H_

#include <string>
#include <deque>
#include <map>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>


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
} jsonValueType_t;


/**  Exception class used by various jsonpp methods */
class JsonException : public std::runtime_error {
  public:
    explicit JsonException ( const std::string & errstr )
        : std::runtime_error(errstr)
    {}
    virtual ~JsonException() throw() {}
    virtual std::string toString() const
    {
        return this->what();
    }
};


/**  JsonItem is the base class defining any JSON
  *  type. 
 **/
class JsonItem {
  public:

    JsonItem ( JsonValueType t = JSON_ITEM ) : _type(t) {}
    virtual ~JsonItem() {}

    JsonValueType getType() const { return _type; }
    JsonValueType getValueType() const { return this->getType(); }

    virtual std::string toString() const;

  protected:

    jsonValueType_t  _type;
};


/** The JsonObject class represents the core JSON type of 
  * an associative array. std::map is used to store all
  * JsonItems.
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


template <typename T>
class JsonType : public JsonItem {
  public:

    JsonType ( const T & val = T(), jsonValueType_t t = JSON_ITEM )
        : JsonItem(t),
          _value(val)
    {}
    JsonType ( const JsonType<T> & val );

    virtual ~JsonType() {}

    JsonType<T>& operator= ( const JsonType<T> & val );
    bool  operator== ( const JsonType<T> & val ) const;

    operator T&() { return this->value(); }
    operator const T&() const { return this->value(); }
    
    T& value() { return _value; }
    const T& value() const { return _value; }

    virtual std::string toString() const;

  private:

    T    _value;
};

typedef JsonType<double>      JsonNumber;
typedef JsonType<bool>        JsonBoolean;
typedef JsonType<std::string> JsonString;

// std::ostream support
std::ostream& operator<< ( std::ostream & stream, const JsonObject  & obj );
std::ostream& operator<< ( std::ostream & stream, const JsonArray   & ary );
std::ostream& operator<< ( std::ostream & stream, const JsonItem    & val );
std::ostream& operator<< ( std::ostream & stream, const JsonNumber  & val );
std::ostream& operator<< ( std::ostream & stream, const JsonBoolean & val );
std::ostream& operator<< ( std::ostream & stream, const JsonString  & str );


// primary interface for parsing json
class JSON {

  public:

    JSON ( const std::string & str = "" ) throw ( JsonException );
    JSON ( const JSON & json );
    ~JSON() throw();

    JSON& operator= ( const JSON & json );

    bool  parse ( const std::string & str );
    void  clear();

    JsonObject& getJSON() { return this->_root; }
    JsonObject& json()    { return this->getJSON(); }

    size_t      getErrorPos() const { return _errpos; }
    std::string getErrorStr ( const std::string & str ) const;

    template<typename T>
    static T    fromString ( const std::string & str );

    static std::string ToString ( const JsonItem * item );
    static std::string typeToString ( JsonValueType  t );

  private:

    bool parseString    ( std::istream & buf, JsonString  & str );
    bool parseArray     ( std::istream & buf, JsonArray   & ary );
    bool parseObject    ( std::istream & buf, JsonObject  & obj );
    bool parseNumber    ( std::istream & buf, JsonNumber  & num );
    bool parseBoolean   ( std::istream & buf, JsonBoolean & b );
    bool parseLiteral   ( std::istream & buf, JsonItem    & item );

    bool parseAssign    ( std::istream & buf );
    bool parseSeparator ( std::istream & buf );

    JsonValueType parseValueType ( std::istream & buf );


  private:

    JsonObject   _root;
    size_t       _errpos;
    
};

} // namespace

#include "JSON.cpp"

#endif  // _JSONPP_JSON_H_

