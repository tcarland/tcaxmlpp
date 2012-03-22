/**
  * @file HexWindow.h
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
#ifndef _HEXES_WINDOW_H_
#define _HEXES_WINDOW_H_

#include <string>

#include "HexApp.h"

namespace hexes {




/**  Provides an Object-oriented interface to a ncurses window. This
  *  class is generally not used directly but is used by HexPanel 
  *  for interacting with the underlying panel window.
 **/
class HexWindow {

    friend class HexPanel;

  public:

    HexWindow();

    HexWindow ( int  height,  int  width,
                bool border = true );

    HexWindow ( int  height,  int  width,
                int  starty,  int  startx,
                bool border = true );

    virtual ~HexWindow();


    int            print ( const std::string & str, bool wrap = true );
    int            print ( const char ch );
    int            echo  ( const char ch );

    void           setWordWrap  ( bool wrap );
    void           setBorder    ( bool show );
    void           drawBorder();
    void           clearBorder();
    void           erase();

    int            width();
    int            height();

    int            startY();
    int            startX();
    int            currentRow();
    int            currentColumn();
    int            curY();
    int            curX();
    HexPosition    currentPosition();

    int            maxColumns();
    int            maxRows();
    int            maxY();
    int            maxX();

    int            move       ( int y,      int x );
    int            moveWindow ( int starty, int startx );
    void           resize     ( int height, int width );

  protected:

    int            wrap();

    static int     LastIndexOf  ( const std::string & str, 
                                  const std::string & match,
                                  size_t from );

    static WINDOW* CreateWindow ( int height, int width, 
                                  int starty, int startx);


  protected:

    WINDOW *    _win;

    int         _height, _width;
    int         _starty, _startx;
    bool        _border;
    bool        _wordwrap;

};

} // namespace

#endif  // _HEXES_WINDOW_H_

