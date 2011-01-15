#define _HEXES_HEXPANEL_CPP_

#include "HexPanel.h"
#include "HexWindow.h"
#include "HexOutputInterface.hpp"
#include "HexInputInterface.hpp"


namespace hexes {

//----------------------------------------------------------------//

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
      _drawBorder(true),
      _drawTitle(true)
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
      _drawBorder(true),
      _drawTitle(true)
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

//----------------------------------------------------------------//

int
HexPanel::poll()
{
    int ch;

    ch = wgetch(_hwin->_win);

    if ( ch == ERR )
        return 0;

    return this->handleInput(ch);
}


int
HexPanel::redraw()
{
    int r = 0;

    _hwin->erase();

    if ( _drawBorder) 
        _hwin->drawBorder();

    if ( _drawTitle)
        mvwaddstr(_hwin->_win, 0, 2, _title.c_str());

    r = this->handleDisplay();

    ::wrefresh(_hwin->_win);

    return r;
}

//----------------------------------------------------------------//


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
        return 0;

    return _input->handleInput(this, ch);
}

//----------------------------------------------------------------//

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

//----------------------------------------------------------------//

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

//----------------------------------------------------------------//
void
HexPanel::erase()
{
    this->_hwin->erase();
}

void
HexPanel::refresh()
{
    ::wrefresh(this->_hwin->_win);
}

//----------------------------------------------------------------//
int
HexPanel::width()
{
    return this->_hwin->width();
}

int
HexPanel::height()
{
    return this->_hwin->height();
}

//----------------------------------------------------------------//


void
HexPanel::setTopPanel()
{
    if ( this->_panel )
        top_panel(this->_panel);
}

TextList&
HexPanel::getTextList()
{
    return this->_textlist;
}

void
HexPanel::setTextList ( TextList & textlist )
{
    this->_textlist = textlist;
}

void
HexPanel::addText ( const std::string & str )
{
    this->_textlist.push_back(str);
}

void
HexPanel::setText ( const std::string & str )
{
    this->_textlist.clear();
    this->addText(str);
}

//----------------------------------------------------------------//

void
HexPanel::enableBorder ( bool border )
{
    this->_drawBorder = border;
}


void
HexPanel::enableTitle ( bool title )
{
    this->_drawTitle = title;
}


void
HexPanel::enableScroll ( bool scroll )
{
    this->_scrollable = scroll;

    if ( this->_hwin == NULL )
        return;

    if ( this->_scrollable )
        ::scrollok(this->_hwin->_win, 1);

    return;
}

bool
HexPanel::scrollable() const
{
    return this->_scrollable;
}

//----------------------------------------------------------------//

void
HexPanel::initPanel()
{
    if ( this->_hwin == NULL )
        return;  // TODO: Throw!

    this->_panel = new_panel(this->_hwin->_win);
    nodelay(this->_hwin->_win, true);
    keypad(this->_hwin->_win, true);
    //top_panel(this->_panel);
}

//----------------------------------------------------------------//

int
HexPanel::print ( const std::string & str )
{
    return _hwin->print(str);
}

int
HexPanel::print ( const char ch )
{
    return _hwin->print(ch);
}

int
HexPanel::echo ( const char ch )
{
    return _hwin->echo(ch);
}

int
HexPanel::wrap()
{
    _hwin->wrap();
}


} // namespace

// _HEXES_HEXPANEL_CPP_

