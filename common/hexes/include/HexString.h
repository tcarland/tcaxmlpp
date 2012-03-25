/**
  * @file HexString.h
  *
  * Copyright (c) 2011 Timothy Charlton Arland
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of libhexes.
  *
  * libhexes is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as
  * published by the Free Software Foundation, either version 3 of
  * the License, or (at your option) any later version.
  *
  * libhexes is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with libhexes.
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#ifndef _HEXES_HEXSTRING_H_
#define _HEXES_HEXSTRING_H_

#include <string> 
#include <sstream>


namespace hexes {


/**  The HexString class wraps a c++ string object while
  *  associating some HexApp attributes such as color
  *  and style attributes from curses, as well as alignment
  *  and wrap behavior used by HexPanel.
 **/
class HexString {

  public:

    typedef std::string::iterator  iterator;
    typedef std::string::size_type size_type;

  public:

    HexString();

    HexString ( const std::string & str,
                bool  wrap = true );

    HexString ( const std::string & str,
                int   color,
                int   attributes,
                bool  wrap  = true );

    HexString ( const HexString & hexstr );

    virtual ~HexString();


    virtual void        operator=   ( const HexString   & hexstr );
    virtual void        operator=   ( const std::string & str );
    virtual bool        operator==  ( const HexString   & hexstr );


    virtual HexString&  append      ( const HexString & hexstr );
    virtual HexString&  append      ( const std::string & str );
    virtual HexString&  append      ( size_type count, char ch );

    virtual HexString&  assign      ( const HexString   & str );
    virtual HexString&  assign      ( const std::string & str );
    virtual HexString&  assign      ( size_type count, char ch );

    virtual iterator    erase       ( iterator   start, iterator end );
    virtual HexString&  erase       ( size_type  from,
                                      size_type  to = std::string::npos );

    virtual size_t      length() const;
    virtual bool        empty()  const;


    std::string&        str();
    const std::string&  str()    const;

    /* some obvious accessor's despite our attributes being public */
    int                 getColor()      const { return color; }
    int                 getAttributes() const { return attributes; }
    int                 getAlignment()  const { return alignment; }
    int                 getWrap()       const { return wrap; }


  public:

    static bool         CharIsVisible ( char ch );

    static int          IndexOf       ( const std::string & str,
                                        const std::string & match,
                                        size_t from );

    /**  Static split function for splitting a string into many parts
      *  based on the provided delimiting character.
     **/
    template< typename OutputIterator_ >
    static inline void  Split         ( const std::string  & str,
                                        const char           delimiter,
                                        OutputIterator_      outI )
    {
        std::string::size_type  begin = 0, end = 0;
        while ( (begin = str.find_first_not_of(delimiter, begin)) != std::string::npos )
        {
            end     = str.find_first_of(delimiter, begin);
            *outI++ = str.substr(begin, end - begin);
            begin   = end;
        }
        return;
    }

    /**  Converts the provided abstract type to a string */
    template<typename T>
    static inline std::string ToString ( const T & a )
    {
        std::stringstream  stream;
        stream << a;
        return stream.str();
    }

    /** Converts a string to the requested abstract type.
      * ie.  int a = HexString::FromString<int>(mystr);
     **/
    template<typename T>
    static inline  T    FromString     ( const std::string & str )
    {
        T target = T();
        std::stringstream stream(str);
        stream >> target;
        return target;
    }


  public:

    int          color;
    int          attributes;
    int          alignment;
    bool         wrap;

  protected:

    std::string  _str;
};

} // namespace

#endif // _HEXES_HEXSTRING_H_

