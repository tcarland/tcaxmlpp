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
      _border(true)
{
    //_win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}

HexWindow::HexWindow ( int height, int width, bool border )
    : _win(NULL),
      _height(height), 
      _width(width),
      _starty(0),
      _startx(0),
      _border(border)
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
      _border(border)
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
    return _win->_maxx;
}

int
HexWindow::height()
{
    return _win->_maxy;
}

void
HexWindow::print ( const std::string & str )
{
    if ( _curP.row >= this->height() )
        return;
    // determine wrap
    mvwaddstr(_win, _curP.row, _curP.col, str.c_str());
    _curP.row++;
}

} // namespace

