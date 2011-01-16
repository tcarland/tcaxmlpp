#define _HEXES_HEXWINDOW_CPP_

#include "HexWindow.h"

namespace hexes {

enum Alignment {
    CENTER = 0,
    LEFT   = 1,
    RIGHT  = 2,
};

HexWindow::HexWindow()
    : _win(NULL),
      _height(5),
      _width(10),
      _starty(0),
      _startx(0),
      _border(true),
      _wrap(true)
{
    //_win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}

HexWindow::HexWindow ( int height, int width, bool border )
    : _win(NULL),
      _height(height), 
      _width(width),
      _starty(0),
      _startx(0),
      _border(border),
      _wrap(true)
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
      _wrap(true)
{
    _win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}


HexWindow::~HexWindow()
{
    if ( _win ) 
    {
        this->erase();
        wrefresh(_win);
        delwin(_win);
    }
}



// TODO: this should assert that the window ptr is not 
// NULL or throw
WINDOW* 
HexWindow::CreateWindow ( int height, int width, 
                          int starty, int startx )
{
    WINDOW * win;
    
    win = newwin(height, width, starty, startx);

    return win;
}

void
HexWindow::setBorder ( bool show )
{
    _border = show;
}

void
HexWindow::drawBorder()
{
    ::box(_win, 0, 0);
}

void
HexWindow::clearBorder()
{
    wborder(_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(_win);
}

void
HexWindow::erase()
{
    ::werase(_win);
}

int
HexWindow::width()
{
    return _width;
}

int
HexWindow::height()
{
    return _height;
}

int
HexWindow::curY()
{
    return _win->_cury;
}

int
HexWindow::curX()
{
    return _win->_curx;
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

HexWindow::Position
HexWindow::currentPosition()
{
    Position p;
    p.row = this->curY();
    p.col = this->curX();
    return p;
}

int
HexWindow::print ( const std::string & str )
{
    if ( _win->_cury >= _height )
        return 0;
    
    std::string s, sstr;
    int    idx;
    int    left = COLS - _win->_curx;
    size_t from = 0;

    s = str;

    if ( left == 0 ) {
        if ( ! this->wrap() )
            return -1;
        left = _width - _win->_curx;
    }

    // do wrap
    while ( s.length() > (size_t) left )
    {
        if ( _wrap ) { // wrap style
            from = left;
            idx  = HexWindow::LastIndexOf(s, " ", from);

            // find the last space
            while ( (idx+1) > left ) {
                if ( idx < 0 )
                   break;
                from = idx;
                idx  = HexWindow::LastIndexOf(s, " ", from);
            }

            if ( idx >= 0 ) { 
                sstr = s.substr(0, idx);
                s.erase(0, idx);
            } else { // no spaces left
                if ( ! this->wrap() )
                    break;
                sstr = s;
                s.erase(0, s.length()-1);
            }
        } else {
            sstr = s.substr(0, left);
            s.erase(0, left);
        }

        waddstr(_win, sstr.c_str());
        if ( ! this->wrap() )
            break;
        left = _width - _win->_curx;
    }

    if ( s.length() > 0 && s.length() < (size_t) left )
        waddstr(_win, s.c_str());

    if ( this->curY() >= _height )
        return 0;

    return(str.length() - s.length());
}


int
HexWindow::wrap()
{
    int curx = _win->_begx + 1;
    int cury = _win->_cury;

    if ( cury >= _height )
        return 0;

    cury++;
    ::wmove(_win, cury, curx);

    return 1;
}


int
HexWindow::print ( const char ch )
{
    if ( _win->_curx >= _width )
        this->wrap();
    return waddch(_win, ch);
}

int
HexWindow::echo ( const char ch )
{
    return wechochar(_win, ch);
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



} // namespace

