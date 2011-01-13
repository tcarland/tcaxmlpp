#define _HEXES_HEXPANEL_CPP_

#include "HexPanel.h"
#include "HexWindow.h"
#include "HexOutputInterface.hpp"
#include "HexInputInterface.hpp"

namespace hexes {


HexPanel::HexPanel ( const std::string & title )
    : _hwin(new HexWindow()),
      _panel(NULL),
      _output(NULL),
      _input(NULL),
      _title(title),
      _height(0),
      _width(0),
      _starty(0),
      _startx(0),
      _maxLines(0),
      _selected(0),
      _scrollable(false),
      _drawBorder(true)
{
    this->initPanel();
}

HexPanel::HexPanel ( const std::string & title,
                     int   height, int   width,
                     int   starty, int   startx )
    :
      _hwin(new HexWindow(height, width, starty, startx)),
      _panel(NULL),
      _output(NULL),
      _input(NULL),
      _title(title),
      _height(0),
      _width(0),
      _starty(0),
      _startx(0),
      _maxLines(0),
      _selected(0),
      _scrollable(false),
      _drawBorder(true)
{
    this->initPanel();
}


HexPanel::~HexPanel()
{
    if ( _hwin )
        delete _hwin;
    this->hide();
    del_panel(_panel);
}


int
HexPanel::redraw()
{
    int r = 0;

    _hwin->erase();

    if ( _drawBorder) 
        _hwin->drawBorder();

    this->setTitle();
    
    r = this->handleDisplay();

    ::wrefresh(_hwin->_win);

    return r;
}



int
HexPanel::handleDisplay()
{
    if ( _output == NULL )
        return 1;

    return _output->handleOutput(this);
}

int
HexPanel::handleInput( int ch )
{
    if ( _input == NULL )
        return 1;

    return _input->handleInput(this, ch);
}


void
HexPanel::setOutputHandler ( HexOutputInterface * output )
{
    this->_output = output;
}

void
HexPanel::setInputHandler ( HexInputInterface * input )
{
    this->_input = input;
}

void
HexPanel::show()
{
    ::show_panel(this->_panel);
}


void
HexPanel::hide()
{
    ::hide_panel(this->_panel);
}

void
HexPanel::erase()
{
    _hwin->erase();
}

void
HexPanel::refresh()
{
    ::wrefresh(_hwin->_win);
}

int
HexPanel::width()
{
    return _hwin->width();
}

int
HexPanel::height()
{
    return _hwin->height();
}

void
HexPanel::setTitle()
{
    if ( _hwin == NULL )
        return;

    mvwaddstr(_hwin->_win, 0, 2, _title.c_str());

    return;
}


void
HexPanel::setTopPanel()
{
    if ( _panel )
        top_panel(_panel);
}


void
HexPanel::enableBorder ( bool border )
{
    _drawBorder = border;
}


void
HexPanel::enableScroll ( bool scroll )
{
    _scrollable = scroll;

    if ( _hwin == NULL )
        return;

    if ( _scrollable )
        ::scrollok(_hwin->_win, 1);

    return;
}

bool
HexPanel::scrollable() const
{
    return _scrollable;
}


void
HexPanel::initPanel()
{
    if ( _hwin == NULL )
        return;  // TODO: Throw!

    _panel = new_panel(_hwin->_win);
    nodelay(_hwin->_win, true);
    keypad(_hwin->_win, true);
    //top_panel(_panel);
}


void
HexPanel::print ( const std::string & str )
{
    _hwin->print(str);
}


} // namespace

// _HEXES_HEXPANEL_CPP_

