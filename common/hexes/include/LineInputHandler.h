/**
  * @file LineInputHandler.h
  *
  * Copyright (c) 2011 Timothy Charlton Arland
  * @author tcarland@gmail.com
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
#ifndef _HEXES_LINEINPUTHANDLER_H_
#define _HEXES_LINEINPUTHANDLER_H_

#include <string>
#include <vector>

#include "HexInputInterface.hpp"


namespace hexes {


#define HEXES_MAXINPUTLEN  2048
#define HEXES_MAXBUFLEN    1024
#define HEXES_DEFBUFLEN    25


/**  A specialized class for handling input as lines.
  *  As input is provided, it is appended to a string until
  *  a carriage return is detected, at which point the
  *  input will be considered ready for consumption.
 **/
class LineInputHandler : public HexInputInterface {

  public:

    LineInputHandler ( int maxlength = HEXES_MAXINPUTLEN );
    virtual ~LineInputHandler();


    virtual int  handleInput ( HexPanel * p, int ch );


    std::string& getLine();

    bool         isReady() const;

    bool         echo() const;
    void         setEcho     ( bool echo );

    int          history() const;
    void         keepHistory ( int lines );

    std::string  prefix() const;
    void         setPrefix   ( const std::string & prompt );

    bool         parse() const;
    void         setParse    ( bool parse );


  protected:

    typedef std::vector<std::string>  InputList;

    std::string  _line;
    std::string  _prompt;

    int          _maxlen;
    bool         _isReady;
    bool         _echo;
    bool         _parse;

    InputList    _history;
    size_t       _lines;
    size_t       _hindx;
};

    
} // namespace

#endif // _HEXES_LINEINPUTHANDLER_H_

