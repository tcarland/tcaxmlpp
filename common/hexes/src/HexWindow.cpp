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
      _startx(0)
{
    _win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}

HexWindow::HexWindow ( int height, int width )
    : _win(NULL),
      _height(height), 
      _width(width),
      _starty(0),
      _startx(0)
{
    _win = HexWindow::CreateWindow(_height, _width, _starty, _startx);
}

HexWindow::HexWindow ( int height, int width,
                       int starty, int startx )
    : _win(NULL),
      _height(height), 
      _width(width),
      _starty(starty), 
      _startx(startx)
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


WINDOW* 
HexWindow::getWindow()
{
    return _win;
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

void
HexWindow::print ( const std::string & str )
{
    //wmove(_win, 1, 1);
    //wprintw(_win, "str: %s", str.c_str());
    //wbkgd(_win, COLOR_PAIR(1));
    //wrefresh(_win);
    mvwaddstr(_win, 1, 1, str.c_str());
}

} // namespace

