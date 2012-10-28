/** @file JSON.cpp
  *
  * Copyright (c) 2012 Timothy Charlton Arland
  * @author tcarland@gmail.com
  * 
  * @section LICENSE
  *
  * 'jsonpp' is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * 'jsonpp' is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with jsonpp.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#define _JSONPP_JSON_CPP_

#include <set>


namespace jsonpp {



inline std::string
JsonItem::toString() const
{
    if ( _type == JSON_NULL ) 
        return std::string("null");

    return std::string("UNKNOWN");
}

// ------------------------------------------------------------------------- //

inline
JsonObject::~JsonObject() throw ()
{
    this->clear();
}

inline
JsonObject::JsonObject ( const JsonObject & obj )
    : JsonItem(JSON_OBJECT)
{
    *this = obj;
}

inline JsonObject&
JsonObject::operator= ( const JsonObject & obj )
{
    if ( this == &obj )
        return *this;

    this->_items.clear();
    this->_type  = obj._type;

    JsonObject::const_iterator jIter;
    for ( jIter = obj.begin(); jIter != obj.end(); ++jIter )
    {
        JsonValueType t = jIter->second->getType();

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

inline JsonItem*
JsonObject::operator[] ( const std::string & key ) throw ( JsonException )
{
    iterator iter;
    
    if ( (iter = this->find(key)) == _items.end() )
        throw ( JsonException("Error, object key not found") );
    return iter->second;
}

inline const JsonItem*
JsonObject::operator[] ( const std::string & key ) const throw ( JsonException )
{
    const_iterator iter = this->find(key);
    if ( iter == _items.end() )
        throw ( JsonException("Error, object key not found") );
    return iter->second;
}

inline JsonObject::pairI
JsonObject::insert ( const std::string & key, JsonItem * item ) throw ( JsonException )
{
    JsonObject::iterator iter;

    if ( (iter = this->find(key)) != _items.end() )
        throw ( JsonException("Item already exists") );
    
    return _items.insert(JsonItems::value_type(key, item));
}

inline void
JsonObject::erase ( JsonObject::iterator at )
{
    if ( at == _items.end() )
        return;
    if ( at->second )
        delete at->second;
    return _items.erase(at);
}

inline void
JsonObject::erase ( JsonObject::iterator first, JsonObject::iterator last )
{
    JsonObject::iterator iter;
    for ( iter = first; iter != last; ++iter ) {
        if ( iter->second )
            delete iter->second;
    }
    return _items.erase(first, last);
}

inline JsonObject::size_type
JsonObject::erase ( const std::string & key )
{
    return _items.erase(key);
}

inline JsonObject::iterator
JsonObject::find ( const std::string & key )
{
    return _items.find(key);
}

inline JsonObject::const_iterator
JsonObject::find ( const std::string & key ) const
{
    return _items.find(key);
}

inline void
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

inline std::string
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

inline
JsonArray::JsonArray()
    : JsonItem(JSON_ARRAY)
{}

inline
JsonArray::JsonArray ( const JsonArray & ary )
    : JsonItem(JSON_ARRAY)
{
    *this = ary;
}

inline
JsonArray::~JsonArray()
{
    this->clear();
}

inline JsonArray&
JsonArray::operator= ( const JsonArray & ary )
{
    if ( this == &ary )
        return *this;

    this->_items.clear();
    this->_type  = ary._type;

    JsonArray::const_iterator  jIter;
    for ( jIter = ary.begin(); jIter != ary.end(); ++jIter )
    {
        JsonValueType t = (*jIter)->getType();
        
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

inline JsonItem*
JsonArray::operator[] ( JsonArray::size_type index )
{
    return _items[index];
}

inline const JsonItem*
JsonArray::operator[] ( JsonArray::size_type index ) const
{
    return _items[index];
}

inline JsonArray::iterator
JsonArray::insert ( JsonItem * item )
{
    return _items.insert(_items.end(), item);
}

inline JsonArray::iterator
JsonArray::insert ( JsonItem * item, JsonArray::iterator at )
{
    return _items.insert(at, item);
}

inline JsonArray::iterator
JsonArray::erase ( JsonArray::iterator at )
{
    return _items.erase(at);
}

inline JsonItem*
JsonArray::at ( JsonArray::size_type index )
{
    return _items.at(index);
}

inline const JsonItem*
JsonArray::at ( JsonArray::size_type index ) const
{
    return _items.at(index);
}

inline void
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

inline std::string
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

template<typename T>
inline 
JsonType<T>::JsonType ( const JsonType<T> & val )
{
    this->_value = val._value;
    this->_type  = val._type;
}

template<typename T>
inline JsonType<T>&
JsonType<T>::operator= ( const JsonType<T> & val )
{
    if ( this != &val ) {
        this->_value = val._value;
        this->_type  = val._type;
    }
    return *this;
}

template<typename T>
inline bool
JsonType<T>::operator== ( const JsonType<T> & val ) const
{
    return(_value == val._value);
}

template<typename T>
inline std::string
JsonType<T>::toString() const
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

// ------------------------------------------------------------------------- //

inline std::ostream&
operator<< ( std::ostream & stream, const JsonObject & obj )
{
    stream << obj.toString();
    return stream;
}

inline std::ostream&
operator<< ( std::ostream & stream, const JsonArray & ary )
{
    stream << ary.toString();
    return stream;
}

inline std::ostream&
operator<< ( std::ostream & stream, const JsonItem & val )
{
    stream << val.toString();
    return stream;
}

inline std::ostream&
operator<< ( std::ostream & stream, const JsonNumber & val )
{
    stream << val.toString();
    return stream;
}

inline std::ostream&
operator<< ( std::ostream & stream, const JsonBoolean & val )
{
    stream << val.toString();
    return stream;
}

inline std::ostream&
operator<< ( std::ostream & stream, const JsonString & str )
{
    stream << str.toString();
    return stream;
}

// ------------------------------------------------------------------------- //

inline
JSON::JSON ( const std::string & str ) throw ( JsonException )
    : _errpos(0)
{
    if ( ! str.empty() && ! this->parse(str) )
        throw ( JsonException("Error parsing string to json") );
}

inline
JSON::JSON ( const JSON & json )
{
    *this = json;
}

inline
JSON::~JSON() throw()
{}

inline JSON&
JSON::operator= ( const JSON & json )
{
    if ( this != &json ) {
        this->_root.clear();
        this->_root   = json._root;
        this->_errpos = json._errpos;
        this->_errstr = json._errstr;
    }
    return *this;
}

inline void
JSON::clear()
{
    this->_root.clear();
}

inline bool
JSON::parse ( const std::string & str )
{
    std::string::size_type  indx;
    bool  p = false;

    indx = str.find_first_of(TOKEN_OBJECT_BEGIN);

    if ( indx == std::string::npos )
        return p;

    std::istringstream sstr(str.substr(indx));
    std::istream       buf(sstr.rdbuf());

    return this->parseObject(buf, _root);
}

inline bool
JSON::parse ( std::istream & buf )
{
    char c;

    while ( !buf.eof() && (c = buf.peek()) != TOKEN_OBJECT_BEGIN )
        buf.get();

    return this->parseObject(buf, _root);
}

inline bool
JSON::parseObject ( std::istream & buf, JsonObject & obj )
{
    bool p = true;
    char c = buf.get();

    if ( c != TOKEN_OBJECT_BEGIN )
        return false;

    JsonObject     jobj;
    JsonArray      jary;
    JsonNumber     jnum;
    JsonBoolean    jbool;
    JsonItem       jnul(JSON_NULL);
    JsonString     jstr, jval;
    JsonValueType  t;
    std::string    key;

    while ( ! buf.eof() )
    {
        c = buf.peek();

        if ( ::isspace(c) ) {
            buf.get();
            continue;
        }
        if ( c == TOKEN_OBJECT_END ) {
            buf.get();
            break;
        }

        // key
        if ( key.empty() ) {
            p = false;
            if ( ! this->parseString(buf, jstr) )
                break;
            key.assign(jstr.value());
            if ( ! this->parseAssign(buf) )
                break;
            continue;
        }

        // val
        t = this->parseValueType(buf);

        switch ( t ) 
        {
            case JSON_OBJECT:
                if ( this->parseObject(buf, jobj) ) {
                    obj.insert(key, new JsonObject(jobj));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_ARRAY:
                if ( this->parseArray(buf, jary) ) {
                    obj.insert(key, new JsonArray(jary));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_NUMBER:
                if ( this->parseNumber(buf, jnum) ) {
                    obj.insert(key, new JsonNumber(jnum));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_STRING:
                if ( this->parseString(buf, jval) ) {
                    obj.insert(key, new JsonString(jval));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_BOOL_TRUE:
            case JSON_BOOL_FALSE:
                if ( this->parseBoolean(buf, jbool) ) {
                    obj.insert(key, new JsonBoolean(jbool));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_NULL:
                if ( this->parseLiteral(buf, jnul) ) {
                    obj.insert(key, new JsonItem(jnul));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_INVALID:
            default:
                return false;
        }
    
        if ( ! p || ! this->parseSeparator(buf) )
            return false;
    }

    return p;
}


inline bool
JSON::parseArray ( std::istream & buf, JsonArray & ary )
{
    bool p = true;
    char c = buf.get();

    if ( c != TOKEN_ARRAY_BEGIN )
        return false;

    JsonArray     jary;
    JsonObject    jobj;
    JsonNumber    jnum;
    JsonBoolean   jbool;
    JsonString    jstr;
    JsonItem      jnul(JSON_NULL);
    JsonValueType t;

    while ( ! buf.eof() )
    {
        c = buf.peek();

        if ( ::isspace(c) ) {
            buf.get();
            continue;
        }

        if ( c == TOKEN_ARRAY_END ) {
            buf.get();
            break;
        }
        
        p = false;
        t = this->parseValueType(buf);
        
        switch ( t ) 
        {
            case JSON_OBJECT:
                if ( this->parseObject(buf, jobj) ) {
                    ary.insert(new JsonObject(jobj));
                    p = true;
                }
                break;
            case JSON_ARRAY:
                if ( this->parseArray(buf, jary) ) {
                    ary.insert(new JsonArray(jary));
                    p = true;
                }
                break;
            case JSON_NUMBER:
                if ( this->parseNumber(buf, jnum) ) {
                    ary.insert(new JsonNumber(jnum));
                    p = true;
                }
                break;
            case JSON_STRING:
                if ( this->parseString(buf, jstr) ) {
                    ary.insert(new JsonString(jstr));
                    p = true;
                }
                break;
            case JSON_BOOL_TRUE:
            case JSON_BOOL_FALSE:
                if ( this->parseBoolean(buf, jbool) ) {
                    ary.insert(new JsonBoolean(jbool));
                    p = true;
                }
                break;
            case JSON_NULL:
                if ( this->parseLiteral(buf, jnul) ) {
                    ary.insert(new JsonItem(jnul));
                    p = true;
                }
                break;
            case JSON_INVALID:
            default:
                this->setError(buf);
                return false;
        }
    
        if ( ! p || ! this->parseSeparator(buf) ) {
            this->setError(buf);
            return false;
        }
    }

    return true;
}


inline bool
JSON::parseString ( std::istream & buf, JsonString & str )
{
    bool start = false;
    bool stop  = false;
    char c;

    std::string sstr;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && ! stop )
    {
        c = buf.get();

        if ( c == TOKEN_STRING_SEPARATOR ) {
            if ( start ) {
                stop = true;
            } else {
                start = true;
                c = buf.get();
            }
        }

        if ( ! start && ! stop ) {
            this->setError(buf);
            return false;
        }

        if ( stop )
            break;

        if ( c == '\\' )
        {
            c = buf.get();
            switch ( c ) 
            {
                case '"':
                case '/':
                case '\\':
                    sstr.push_back(c);
                    break;
                case 'b':
                    sstr.push_back('\b');
                    break;
                case 'f':
                    sstr.push_back('\f');
                    break;
                case 'n':
                    sstr.push_back('\n');
                    break;
                case 'r':
                    sstr.push_back('\r');
                    break;
                case 't':
                    sstr.push_back('\t');
                    break;
                default:   // error
                    this->setError(buf);
                    return false;
                    break;
            }
        } 
        else 
        {
            sstr.push_back(c);
        }
    }

    str = JsonString(sstr, JSON_STRING);

    return true;
}

inline bool
JSON::parseNumber ( std::istream & buf, JsonNumber & num )
{
    const char nums[] = "-+.eE0123456789";
    std::set<char> numset;
    std::string    numstr;

    for ( size_t i = 0; i < sizeof(nums); ++i )
        numset.insert(nums[i]);

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && numset.find(buf.peek()) != numset.end() )
        numstr.push_back(buf.get());

    if ( numstr.empty() ) {
        this->setError(buf);
        return false;
    }

    num = JsonNumber(JSON::FromString<double>(numstr), JSON_NUMBER);

    return true;
}

inline bool
JSON::parseBoolean ( std::istream & buf, JsonBoolean & b )
{
    std::string token;
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && buf.peek() != TOKEN_VALUE_SEPARATOR ) 
    {
        c = buf.get();
        if ( ::isspace(c) )
            continue;
        token.push_back(c);
    }

    if ( token.compare("true") == 0 ) {
        b = JsonBoolean(true, JSON_BOOL_TRUE);
        return true;
    }

    if ( token.compare("false") == 0 ) {
        b = JsonBoolean(false, JSON_BOOL_FALSE);
        return true;
    }

    this->setError(buf);

    return false;
}

inline bool
JSON::parseLiteral ( std::istream & buf, JsonItem & item )
{
    std::string token;
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && buf.peek() != TOKEN_VALUE_SEPARATOR ) 
    {
        c = buf.get();
        if ( ::isspace(c) )
            continue;
        token.push_back(c);
    }

    if ( item.getType() == JSON_NULL && token.compare("null") == 0 )
        return true;
    
    this->setError(buf);

    return false;
}

inline bool
JSON::parseAssign ( std::istream & buf )
{
    char  c;
    
    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();
    
    c = buf.get();

    if ( c == TOKEN_NAME_SEPARATOR ) 
        return true;

    this->setError(buf);

    return false;
}


inline bool
JSON::parseSeparator ( std::istream & buf )
{
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();
    
    if ( (c = buf.peek()) == TOKEN_VALUE_SEPARATOR ) {
        buf.get();
        return true;
    }

    if ( c == TOKEN_ARRAY_END || c == TOKEN_OBJECT_END )
        return true;
    
    this->setError(buf);

    return false;
}


inline JsonValueType
JSON::parseValueType ( std::istream & buf )
{
    JsonValueType t;
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    c = buf.peek();

    switch ( c ) {
        case TOKEN_ARRAY_BEGIN:
            t = JSON_ARRAY;
            break;
        case TOKEN_OBJECT_BEGIN:
            t = JSON_OBJECT;
            break;
        case TOKEN_STRING_SEPARATOR:
            t = JSON_STRING;
            break;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            t = JSON_NUMBER;
            break;
        case 't':  
            t = JSON_BOOL_TRUE;
            break;
        case 'f':  
            t = JSON_BOOL_FALSE;
            break;
        case 'n':
            t = JSON_NULL;
            break;
        default: 
            t = JSON_INVALID;
            break;
    }

    return t;
}

inline void
JSON::setError ( std::istream & buf )
{
    _errpos = buf.tellg();
    _errstr.clear();
    if ( _errpos < 15 )
        buf.seekg(0);
    else
        buf.seekg(_errpos - 15);
    while ( ! buf.eof() && buf.tellg() < (_errpos + 5) )
        _errstr.push_back(buf.get());
    return;
}

inline std::string
JSON::TypeToString ( JsonValueType t )
{
    std::string name;

    switch ( t ) {
        case JSON_ITEM:
            name.assign("Unknown");
            break;
        case JSON_OBJECT:
            name.assign("Json Object");
            break;
        case JSON_ARRAY:
            name.assign("Json Array");
            break;
        case JSON_NUMBER:
            name.assign("Json Number");
            break;
        case JSON_STRING:
            name.assign("Json String");
            break;
        case JSON_BOOL_TRUE:
        case JSON_BOOL_FALSE:
        case JSON_NULL:
            name.assign("Json Literal");
            break;
        case JSON_INVALID:
        default:
            name.assign("Invalid type");
            break;
    }

    return name;
}


template<typename T>
inline T 
JSON::FromString ( const std::string & str )
{
    T target = T();
    std::stringstream strm(str);
    strm >> target;
    return target;
}

inline std::string
JSON::ToString ( const JsonItem * item )
{
    JsonValueType t = item->getType();

    if ( t == JSON_ITEM )
        return item->toString();
    else if ( t == JSON_OBJECT ) {
        const JsonObject * obj = (const JsonObject*) item;
        return obj->toString();
    } else if ( t == JSON_ARRAY ) {
        const JsonArray  * ary = (const JsonArray*) item;
        return ary->toString();
    } else if ( t == JSON_NUMBER ) {
        const JsonNumber * num = (const JsonNumber*) item;
        return num->toString();
    } else if ( t == JSON_STRING ) {
        const JsonString * str = (const JsonString*) item;
        return str->toString();
    } else if ( t == JSON_BOOL_TRUE || t == JSON_BOOL_FALSE ) {
        const JsonBoolean * jb = (const JsonBoolean*) item;
        return jb->toString();
    } else if ( t == JSON_NULL ) {
        return item->toString();
    }

    return std::string("Invalid type");
}

inline std::string
JSON::Version()
{
    std::string ver = "jsonpp v";
    ver.append(JSONPP_VERSION);
    return ver;
}

} // namespace


