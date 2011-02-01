#define _HEXES_HEXPANEL_CPP_
#include <sstream>

#include "HexPanel.h"
#include "HexWindow.h"
#include "HexOutputInterface.hpp"
#include "HexInputInterface.hpp"
#include "LineOutputHandler.h"

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
      _selected(0),
      _maxLines(DEFAULT_SCRLBK_SIZE),
      _scrollTo(0),
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
      _hwin(new HexWindow(height, width, starty, startx, true)),
      _panel(NULL),
      _output(new LineOutputHandler()),
      _input(NULL),
      _title(title),
      _height(height),
      _width(width),
      _starty(starty),
      _startx(startx),
      _selected(0),
      _maxLines(DEFAULT_SCRLBK_SIZE),
      _scrollTo(0),
      _scrollable(false),
      _drawBorder(true),
      _drawTitle(true)
{
    this->initPanel();
}


HexPanel::~HexPanel()
{
    this->hide();

    ::del_panel(_panel);

    if ( _hwin )
        delete _hwin;
    if ( _output )
        delete _output;
    if ( _input )
        delete _input;
}

//----------------------------------------------------------------//

int
HexPanel::poll()
{
    int ch;

    ch = wgetch(_hwin->_win);

    if ( ch == ERR )
        return ch;

    if ( _input == NULL )
        return ch;

    return _input->handleInput(this, ch);
}


int
HexPanel::redraw()
{
    int r = 0;

    _hwin->erase();

    if ( _output != NULL )
        r = _output->handleOutput(this);

    if ( _drawBorder) 
        _hwin->drawBorder();
    if ( _drawTitle)
        mvwaddstr(_hwin->_win, 0, 2, _title.c_str());

    ::wmove(_hwin->_win, 1, 1);
    ::wrefresh(_hwin->_win);

    return r;
}

void
HexPanel::resize ( int height, int width )
{
    this->_height = height;
    this->_width  = width;
    ::wresize(_hwin->_win, height, width);
    if ( this->curY() > height || this->curX() > width )
        this->move(height, width);
    return;
}

//----------------------------------------------------------------//

void
HexPanel::setOutputHandler ( HexOutputInterface * output )
{
    if ( this->_output )
        delete this->_output;
    this->_output = output;
}

HexOutputInterface*
HexPanel::getOutputHandler()
{
    return this->_output;
}

void
HexPanel::setInputHandler ( HexInputInterface * input )
{
    if ( this->_input )
        delete this->_input;
    this->_input = input;
}

HexInputInterface*
HexPanel::getInputHandler()
{
    return _input;
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
//
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

int
HexPanel::curY()
{
    return this->_hwin->curY();
}

int
HexPanel::curX()
{
    return this->_hwin->curX();
}

int
HexPanel::maxY()
{
    return this->_hwin->maxY();
}

int
HexPanel::maxX()
{
    return this->_hwin->maxX();
}


//----------------------------------------------------------------//

void
HexPanel::setTopPanel()
{
    if ( this->_panel )
        top_panel(this->_panel);
}

//----------------------------------------------------------------//

/**  Returns a reference to the internal TextList */
TextList&
HexPanel::getTextList()
{
    return this->_textlist;
}

/**  Sets the internal TextList to be a copy of @param textlist */
void
HexPanel::setTextList ( TextList & textlist )
{
    this->_textlist = textlist;
}

/** addText will add the provided string as the next 
 *  line of text of the panel. Each string is considered 
 *  a single line, and the cursor will automatically wrap
 *  after each string.
 **/
void
HexPanel::addText ( const std::string & str )
{
    _textlist.push_back(str);

    if ( _textlist.size() >= ((size_t) _height - 1) )
    {
        if ( ! _scrollable || _textlist.size() > ((size_t) _height + _maxLines) )
            _textlist.pop_front();
    }

    return;
}

/** setText clears the internal TextList and sets the
  * given @param str as the first line of text.
 **/
void
HexPanel::setText ( const std::string & str )
{
    this->_textlist.clear();
    this->addText(str);
}

/**  Clears the internal TextList */
void
HexPanel::clearText()
{
    this->_textlist.clear();
}

void
HexPanel::clear()
{
    return this->clearText();
}

//----------------------------------------------------------------//

void
HexPanel::drawBorder ( bool border )
{
    this->_drawBorder = border;
    this->_hwin->setBorder(border);
}

void
HexPanel::drawTitle ( bool title )
{
    this->_drawTitle = title;
}

bool
HexPanel::drawBorder() const
{
    return this->_drawBorder;
}

bool
HexPanel::drawTitle() const
{
    return this->_drawTitle;
}

const std::string&
HexPanel::getTitle() const
{
    return this->_title;
}

//----------------------------------------------------------------//

bool
HexPanel::enableScroll ( bool scroll, int lines )
{
    this->_scrollable = scroll;

    if ( this->_hwin == NULL )
        return false;

    if ( this->_scrollable )
        ::scrollok(this->_hwin->_win, 1);

    if ( lines > MAX_SCRLBK_SIZE ) {
        _maxLines = MAX_SCRLBK_SIZE;
        return false;
    } else {
        _maxLines = lines;
    }

    return true;
}

bool
HexPanel::setMaxLines ( int lines )
{
    _maxLines = lines;
    if ( lines > MAX_SCRLBK_SIZE ) {
        _maxLines = MAX_SCRLBK_SIZE;
        return false;
    }
    return true;
}

void
HexPanel::scrollLine()
{
    ::wscrl(_hwin->_win, 1);
}

bool
HexPanel::scrollable() const
{
    return this->_scrollable;
}

//----------------------------------------------------------------//

int
HexPanel::print ( const std::string & str, bool wrap )
{
    return _hwin->print(str, wrap);
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

//----------------------------------------------------------------//

int
HexPanel::wrap()
{
    return _hwin->wrap();
}

//----------------------------------------------------------------//

/** Sets a timeout delay for a given window when polling for
  * input. Using a value of -1 will make poll() block.
 **/
void
HexPanel::timeout ( int delay_ms )
{
    ::wtimeout(_hwin->_win, delay_ms);
}

//----------------------------------------------------------------//

int
HexPanel::move ( int y , int x )
{
    return(::wmove(_hwin->_win, y, x));
}

int
HexPanel::move ( HexPosition & p )
{
    return(::wmove(_hwin->_win, p.row, p.col));
}

int
HexPanel::moveWindow ( int starty, int startx )
{
    return(::mvwin(_hwin->_win, starty, startx));
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

    // set default input timeout?
    this->timeout(-1); // blocking input

    return;
}

//----------------------------------------------------------------//

} // namespace

// _HEXES_HEXPANEL_CPP_

