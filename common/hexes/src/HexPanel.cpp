#define _HEXES_HEXPANEL_CPP_
#include <sstream>

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
      _selected(0),
      _maxLines(DEFAULT_SCROLLBACK),
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
      _hwin(new HexWindow(height, width, starty, startx)),
      _panel(NULL),
      _output(NULL),
      _input(NULL),
      _title(title),
      _height(height),
      _width(width),
      _starty(starty),
      _startx(startx),
      _selected(0),
      _maxLines(DEFAULT_SCROLLBACK),
      _scrollTo(0),
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

    return this->handleInput(ch);
}


int
HexPanel::redraw()
{
    int r = 0;

    _hwin->erase();
    
    r = this->handleDisplay();

    if ( _drawBorder) 
        _hwin->drawBorder();
    if ( _drawTitle)
        mvwaddstr(_hwin->_win, 0, 2, _title.c_str());

    ::wmove(_hwin->_win, 1, 1);
    ::wrefresh(_hwin->_win);

    return r;
}

//----------------------------------------------------------------//


int
HexPanel::handleDisplay()
{
    if ( _output != NULL )
        return _output->handleOutput(this);

    TextList::iterator  tIter;
    TextList & tlist = this->getTextList();

    int ht = _height;
    int ln = 1;

    if ( _drawBorder ) {
        ht -= 2;
        this->move(1, 1);
    }

    if ( _scrollTo == 0 )
        _scrollTo = tlist.size();

    for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter, ++ln )
    {
        if ( ln > ht && _scrollable )
            this->scrollLine();

        this->print(*tIter);

        if ( ln < ht && (size_t) ln != tlist.size() )
            this->wrap();
        else
            this->move(this->curY(), 1);
    }

    return 1;
}

int
HexPanel::handleInput( int ch )
{
    if ( _input == NULL )
        return ch;

    return _input->handleInput(this, ch);
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

void
HexPanel::setText ( const std::string & str )
{
    this->_textlist.clear();
    this->addText(str);
}

//----------------------------------------------------------------//

void
HexPanel::drawBorder ( bool border )
{
    this->_drawBorder = border;
}

void
HexPanel::drawTitle ( bool title )
{
    this->_drawTitle = title;
}

const std::string&
HexPanel::getTitle() const
{
    return this->_title;
}

void
HexPanel::enableScroll ( bool scroll, int lines )
{
    this->_scrollable = scroll;

    if ( this->_hwin == NULL )
        return;

    if ( this->_scrollable )
        ::scrollok(this->_hwin->_win, 1);

    if ( lines > MAX_SCROLLBACK_SIZE ) 
        _maxLines = MAX_SCROLLBACK_SIZE;
    else
        _maxLines = lines;

    return;
}

void
HexPanel::setMaxLines ( int lines )
{
    _maxLines = lines;
    if ( lines > MAX_SCROLLBACK_SIZE ) 
        _maxLines = MAX_SCROLLBACK_SIZE;
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

int
HexPanel::wrap()
{
    return _hwin->wrap();
}

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

