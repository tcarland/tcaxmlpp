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
      _output(new LineOutputHandler()),
      _input(NULL),
      _title(title),
      _panelId(0),
      _selected(0),
      _maxLines(DEFAULT_SCRLBK_SIZE),
      _scrollTo(0),
      _txtColor(0), _bdrColor(0), _bfgColor(0),
      _bdrAttr(0),
      _scrollable(false),
      _drawBorder(true), _drawTitle(true),
      _focus(false)
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
      _panelId(0),
      _selected(0),
      _maxLines(DEFAULT_SCRLBK_SIZE),
      _scrollTo(0),
      _txtColor(0), _bdrColor(0), _bfgColor(0),
      _bdrAttr(0),
      _scrollable(false),
      _drawBorder(true), _drawTitle(true),
      _focus(false)
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

    if ( _drawBorder) {
        if ( _bfgColor > 0 && _focus )
	    wattrset(_hwin->_win, COLOR_PAIR(_bfgColor));
        else
	    wattrset(_hwin->_win, COLOR_PAIR(_bdrColor));
        this->setAttribute(_bdrAttr);
        _hwin->drawBorder();
        this->unsetAttribute(_bdrAttr);
    }

    if ( _drawTitle) {
        if ( _winTitle.empty() )
            _winTitle = _title;
        wattrset(_hwin->_win, COLOR_PAIR(_winTitle.color));
        this->setAttribute(_winTitle.attributes);
        mvwaddstr(_hwin->_win, 0, 2, _winTitle.str().c_str());
        this->unsetAttribute(_winTitle.attributes);
    }

    wattrset(_hwin->_win, COLOR_PAIR(_txtColor));

    ::wmove(_hwin->_win, 1, 1);
    ::wrefresh(_hwin->_win);

    return r;
}

void
HexPanel::resize ( int height, int width )
{
    return(_hwin->resize(height, width));
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

/** Returns the current width of the window (including border) */
int
HexPanel::width()
{
    return this->_hwin->width();
}

/** Returns the current height of the window (including border) */
int
HexPanel::height()
{
    return this->_hwin->height();
}

/** Returns the current Y starting coordinate of the window */
int
HexPanel::startY()
{
    return this->_hwin->startY();
}

/** Returns the current X starting coordinate of the window */
int
HexPanel::startX()
{
    return this->_hwin->startX();
}

/** Returns the current Y coordinate of the window */
int
HexPanel::curY()
{
    return this->_hwin->curY();
}

/** Returns the current X coordinate of the window */
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

/** Sets the current panel instance as the top panel */
void
HexPanel::setFocus()
{
    if ( this->_panel )
        ::top_panel(this->_panel);
    _focus = true;
}

/** Tells this panel that it no longer has focus */
void
HexPanel::unsetFocus()
{
    _focus = false;
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

/**  Return the number of lines stored by the internal TextList */
int
HexPanel::getLineCount()
{
    return this->_textlist.size();
}

//----------------------------------------------------------------//

/**@{
  *  addText will add the provided string as the next
  *  line of text of the panel. Each string is considered
  *  a single line, and the cursor will automatically wrap
  *  after each string.
 **/
void
HexPanel::addText ( const std::string & str )
{
    HexString  line(str, _txtColor, HEX_NORMAL);
    return this->addText(line);
}

void
HexPanel::addText ( const std::string & str, int color, int attr )
{
    HexString  line(str, color, attr);
    return this->addText(line);
}

void
HexPanel::addText ( const HexString & hexstr )
{
    _textlist.push_back(hexstr);

    if ( _textlist.size() >= ((size_t)this->height()-1) )
    {
        if ( ! _scrollable || _textlist.size() > ((size_t)this->height()+_maxLines) )
            _textlist.pop_front();
    }

    return;
}
/*@}*/

//----------------------------------------------------------------//

/**@{
  *  setText clears the internal TextList and sets the
  * given @param str as the first line of text.
 **/
void
HexPanel::setText ( const std::string & str )
{
    return this->setText(str, _txtColor, HEX_NORMAL);
}

void
HexPanel::setText ( const std::string & str, int color, int attr )
{
    this->_textlist.clear();
    this->addText(str, color, attr);
}

void
HexPanel::setText ( const HexString & hexstr )
{
    this->_textlist.clear();
    this->addText(hexstr);
}
/*@}*/

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

/** Returns the current border color pair value */
int
HexPanel::getBorderColor()
{
    return _bdrColor;
}

/** Sets the border's color pair index value */
void
HexPanel::setBorderColor ( int colorIndex )
{
    _bdrColor = colorIndex;
}

/** Returns the border's active color pair index value */
int
HexPanel::getBorderActiveColor()
{
    return _bfgColor;
}

/** Sets the border's active color (when focused) */
void
HexPanel::setBorderActiveColor ( int colorIndex )
{
    _bfgColor = colorIndex;
}

/** Return the current border's attributes value */
int
HexPanel::getBorderAttributes()
{
    return _bdrAttr;
}

/** Set the attributes for this panel's border */
void
HexPanel::setBorderAttributes ( int attr )
{
    _bdrAttr = attr;
}

int
HexPanel::getTextColor()
{
    return _txtColor;
}

/** Set the default color for this panel's text */
void
HexPanel::setTextColor ( int colorIndex )
{
    _txtColor = colorIndex;
}

//----------------------------------------------------------------//
//
void
HexPanel::setAttribute ( int attr )
{
    ::wattron(_hwin->_win, attr);
}

void
HexPanel::unsetAttribute ( int attr )
{
    ::wattroff(_hwin->_win, attr);
}

//----------------------------------------------------------------//

/** Enables or disables the drawing of the window border */
void
HexPanel::setDrawBorder ( bool border )
{
    this->_drawBorder = border;
    this->_hwin->setBorder(border);
}

/** Enables or disables the drawing of the window title */
void
HexPanel::setDrawTitle ( bool title )
{
    this->_drawTitle = title;
}

bool
HexPanel::getDrawBorder() const
{
    return this->_drawBorder;
}

bool
HexPanel::getDrawTitle() const
{
    return this->_drawTitle;
}

void
HexPanel::setWindowTitle ( HexString & hexstr )
{
    _winTitle = hexstr;
}

void
HexPanel::setWindowTitle ( const std::string & str, int color, int attr )
{
    _winTitle = HexString(str, color, attr);
}

HexString&
HexPanel::getWindowTitle()
{
    return this->_winTitle;
}

const std::string&
HexPanel::getPanelName() const
{
    return this->_title;
}

void
HexPanel::setPanelId ( int panelId )
{
    _panelId = panelId;
}

int
HexPanel::getPanelId() const
{
    return this->_panelId;
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

/**  Prints a given string and attributes to the panel.
  * @param str   is the string to print to this panel.
  * @param pos   is the window location to start printing.
  * @param color is the color pair index for the string.
  * @param attr  is the text attributes for the string.
  * @param wrap  determines whether the string should wrap.
 */
int
HexPanel::print ( const std::string & str, HexPosition & pos,
                  int color, int attr, bool wrap )
{
    int r = 0;

    this->move(pos);
    this->setAttribute(COLOR_PAIR(color));
    if ( attr > HEX_NORMAL )
        this->setAttribute(attr);

    r = _hwin->print(str, wrap);

    this->unsetAttribute(COLOR_PAIR(color));
    if ( attr > HEX_NORMAL )
        this->unsetAttribute(attr);

    return r;
}

int
HexPanel::print ( HexString & str, HexPosition & pos )
{
    return this->print(str.str(), pos, str.color, str.attributes, str.wrap);
}

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
    return(_hwin->move(y, x));
}

int
HexPanel::move ( HexPosition & p )
{
    return(_hwin->move(p.row, p.col));
}

int
HexPanel::moveWindow ( int starty, int startx )
{
    return(_hwin->moveWindow(starty, startx));
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

