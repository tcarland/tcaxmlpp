/**
  * @file HexWindow.cpp
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
#define _HEXES_HEXWINDOW_CPP_

#include "HexWindow.h"
#include "HexPosition.hpp"


namespace hexes {


HexWindow::HexWindow()
    : _win(NULL),
      _height(5),
      _width(10),
      _starty(0),
      _startx(0),
      _border(false),
      _wordwrap(false)
{
    _win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}

HexWindow::HexWindow ( int height, int width, bool border )
    : _win(NULL),
      _height(height), 
      _width(width),
      _starty(0),
      _startx(0),
      _border(border),
      _wordwrap(false)
{
    _win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}

HexWindow::HexWindow ( int height, int width,
                       int starty, int startx,
                       bool border )
    : _win(NULL),
      _height(height), 
      _width(width),
      _starty(starty), 
      _startx(startx),
      _border(border),
      _wordwrap(false)
{
    _win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}


HexWindow::~HexWindow()
{
    if ( _win ) {
        this->erase();
        ::wrefresh(_win);
        ::delwin(_win);
    }
}

//----------------------------------------------------------------//

void
HexWindow::setWordWrap ( bool wrap )
{
    this->_wordwrap = wrap;
}

void
HexWindow::setBorder ( bool show )
{
    this->_border = show;
}

void
HexWindow::drawBorder()
{
    _border = true;
    ::box(_win, 0, 0);
}

void
HexWindow::clearBorder()
{
    ::wborder(_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    ::wrefresh(_win);
}

//----------------------------------------------------------------//

/** Erases the window making it invisible */
void
HexWindow::erase()
{
    ::werase(_win);
}

/** Returns the current width of the window */
int
HexWindow::width()
{
    return this->_width;
}

/** Returns the current height of the window */
int
HexWindow::height()
{
    return this->_height;
}

//----------------------------------------------------------------//

/** Returns the current Y starting coordinate of the window */
int
HexWindow::startY()
{
    return this->_starty;
}

/** Returns the current X starting coordinate of the window */
int
HexWindow::startX()
{
    return this->_startx;
}

int
HexWindow::curY()
{
    return this->_win->_cury;
}

int
HexWindow::curX()
{
    return this->_win->_curx;
}

int
HexWindow::currentColumn()
{
    return this->curX();
}

int
HexWindow::currentRow()
{
    return this->curY();
}

//----------------------------------------------------------------//

int
HexWindow::maxY()
{
    return this->_win->_maxy;
}

int
HexWindow::maxX()
{
    return this->_win->_maxx;
}

int
HexWindow::maxRows()
{
    return this->maxY();
}

int
HexWindow::maxColumns()
{
    return this->maxX();
}

/**  Returns the current window cursor position */
HexPosition
HexWindow::currentPosition()
{
    HexPosition  p;
    p.row = this->curY();
    p.col = this->curX();
    return p;
}

/**  Moves the cursor position within the current window.
  *  The position is relative to the upper left corner 
  *  of the window considered (0, 0).
  *  Returns 0 on success or an ERR value upon failure.
 **/
int
HexWindow::move ( int y, int x )
{
    return(::wmove(_win, y, x));
}

/**  Moves the window so that the upper left corner of
  *  the window starts at the provided coordinates.
 **/
int
HexWindow::moveWindow ( int starty, int startx )
{
    this->_starty = starty;
    this->_startx = startx;
    return(::mvwin(_win, starty, startx));
}

/**  Resizes the window to the provided height and width. */
void
HexWindow::resize ( int height, int width )
{
    this->_height = height;
    this->_width  = width;

    if ( this->curY() > height || this->curX() > width )
        this->move(height, width);

    ::wresize(_win, height, width);

    return;
}


//----------------------------------------------------------------//

int
HexWindow::print ( const std::string & str, bool wrap )
{

    std::string  s, sstr;
    int          wd, ht, idx, left;
    size_t       from;
    
    s    = str;
    wd   = _width;
    ht   = _height;
    left = _width - this->curX();
    from = 0;

    if ( _border ) {
        ht   -= 2;
        wd   -= 2;
        left -= 2;
    }

    if ( this->curY() > ht )
        return 0;

    if ( _win->_curx == 0 && _border )
        ::wmove(_win, this->curY(), 1);

    if ( left <= 1 ) {
        if ( ! this->wrap() )
            return -1;
        left = wd - this->curX();
    }

    // wrap logic needed to keep curses 
    // from doing its own wrap and hammering a border
    while ( s.length() > (size_t) left )
    {
        if ( _wordwrap )  // wrap style
        {
            from = left;
            idx  = HexWindow::LastIndexOf(s, " ", from);

            while ( idx > left ) // find the last space
            {
                if ( idx < 0 )
                   break;
                from = idx;
                idx  = HexWindow::LastIndexOf(s, " ", from);
            }

            if ( idx > 0 ) { 
                sstr = s.substr(0, idx);
                s.erase(0, idx);
            } else { // no spaces left
                if ( ! this->wrap() )
                    break;
                sstr = s;
                s.erase(0, s.length()-1);
            }
        } 
        else 
        {
            sstr = s.substr(0, left);
            s.erase(0, left);
        }

        ::waddstr(_win, sstr.c_str());

        if ( ! this->wrap() )
            break;
        left = wd - this->curX();
    }

    if ( s.length() > 0 && s.length() < (size_t) left )
        ::waddstr(_win, s.c_str());

    if ( wrap )
        this->wrap();

    if ( this->curY() >= ht )
        return 0;

    return(str.length() - s.length());
}


int
HexWindow::print ( const char ch )
{
    if ( this->curX() >= _width )
        this->wrap();
    return(::waddch(_win, ch));
}


int
HexWindow::echo ( const char ch )
{
    return(::wechochar(_win, ch));
}


int
HexWindow::wrap()
{
    int curx = 0;
    int cury = this->curY();

    if ( cury >= _height )
        return 0;

    cury++;
    ::wmove(_win, cury, curx);

    return 1;
}

//----------------------------------------------------------------//


WINDOW* 
HexWindow::CreateWindow ( int height, int width, 
                          int starty, int startx )
{
    WINDOW * win;
    
    win = ::newwin(height, width, starty, startx);

    return win;
}


int
HexWindow::LastIndexOf ( const std::string & str, const std::string & match, size_t from )
{
    std::string::size_type  indx;

    if ( from > str.length() )
	from = str.length();
    else if ( from < 0 )
	return -1;

    if ( (indx = str.find_last_of(match, from)) == std::string::npos )
	return -1;

    return ( (int) indx );
}

//----------------------------------------------------------------//


} // namespace

