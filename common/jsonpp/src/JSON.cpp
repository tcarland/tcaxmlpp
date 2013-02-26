/**
  * @file JSON.cpp
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
#define _JSONPP_JSON_CPP_

#include <set>

#include "JSON.h"


namespace jsonpp {

// ------------------------------------------------------------------------- //

std::ostream&
operator<< ( std::ostream & strm, const JsonObject & obj )
{
    strm << obj.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonArray & ary )
{
    strm << ary.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonItem & val )
{
    strm << val.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonNumber & val )
{
    strm << val.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonBoolean & val )
{
    strm << val.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonString & str )
{
    strm << str.toString();
    return strm;
}

// ------------------------------------------------------------------------- //

JSON::JSON ( const std::string & str ) throw ( JsonException )
    : _errpos(0)
{
    if ( ! str.empty() && ! this->parse(str) )
        throw ( JsonException("Error parsing string to json") );
}

JSON::JSON ( const JSON & json )
{
    *this = json;
}

JSON::~JSON() throw()
{}

JSON&
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

void
JSON::clear()
{
    this->_root.clear();
}

bool
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

bool
JSON::parse ( std::istream & buf )
{
    char c;

    while ( !buf.eof() && (c = buf.peek()) != TOKEN_OBJECT_BEGIN )
        buf.get();

    return this->parseObject(buf, _root);
}

bool
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


bool
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


bool
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

bool
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

bool
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

bool
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

bool
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


bool
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


json_t
JSON::parseValueType ( std::istream & buf )
{
    json_t t;
    char   c;

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

void
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

std::string
JSON::TypeToString ( json_t t )
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

std::string
JSON::ToString ( const JsonItem * item )
{
    json_t  t = item->getType();

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

std::string
JSON::Version()
{
    std::string ver = "jsonpp v";
    ver.append(JSONPP_VERSION);
    return ver;
}


} // namespace

// _JSONPP_JSON_CPP_

