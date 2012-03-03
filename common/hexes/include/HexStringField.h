/**
  * @file HexStringField.h
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
#ifndef _HEXSTRINGFIELD_H_
#define _HEXSTRINGFIELD_H_

#include "HexString.h"

namespace hexes {


/**  A custom HexString class that creates an entry field for a
  *  string and displays '_' as the field for the given length.
  *
  *  TODO:  Allow field to have title like USER: ______
  *         support scrolling within the field for longer strings.
 */
class HexStringField : public HexString {

  public:

    HexStringField ( size_t length = 0 );
    HexStringField ( const HexString   & str, size_t length = 0 );
    HexStringField ( const std::string & str, size_t length = 0 );

    virtual ~HexStringField();

    virtual void        operator=  ( const HexString   & hexstr );
    virtual void        operator=  ( const std::string & str );

    virtual HexString&  append     ( const HexString   & hexstr );
    virtual HexString&  append     ( const std::string & str );
    virtual HexString&  append     ( size_type count, char ch );

    virtual HexString&  assign     ( const HexString   & hexstr );
    virtual HexString&  assign     ( const std::string & str );
    virtual HexString&  assign     ( size_type count, char ch );

    virtual iterator    erase      ( iterator start, iterator end );
    virtual HexString&  erase      ( size_type  from,
                                     size_type  to = std::string::npos );

    virtual size_t      length() const;


  protected:

    size_t  _maxLen;
    size_t  _curLen;
    
};

} // namespace

#endif  // _HEXSTRINGFIELD_H_

