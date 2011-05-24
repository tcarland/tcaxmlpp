/**
  * @file HexDialog.cpp
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
#define _HEXES_HEXDIALOG_CPP_

#include "HexDialog.h"


namespace hexes {


/**  Constructs a dialog panel with no default dimensions.
  * The dialog will dynamically size itself based on the 
  * contents.
 **/
HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog )
    : HexPanel(title, 0, 0, 0, 0),
      _rescnt(0),
      _dynsz(true),
      _char(true),
      _echo(false)
{
    if ( ! dialog.empty() )
        this->addText(dialog);
}

/**  Constructs a dialog panel with fixed dimensions and 
  * starting location. The dialog will NOT be dynamically 
  * sized.
 **/
HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog,
                       int   height, int   width,
                       int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _rescnt(0),
      _dynsz(false),
      _char(true),
      _echo(false)
{
    if ( ! dialog.empty() )
        this->addText(dialog);
}


HexDialog::~HexDialog() {}


/**  Displays the modal dialog window. */
int
HexDialog::showDialog()
{
    int  ch;
    bool echo = _echo;

    this->initDialog();
    this->enableScroll(false);
    this->setFocus();
    this->redraw();

    this->setInputHandler(new LineInputHandler(_rescnt));
    LineInputHandler * cinput = (LineInputHandler*) this->getInputHandler();
    cinput->setEcho(false);

    ch = this->poll();

    while ( ! cinput->isReady() ) 
    {
        if ( _char )
            break;

        if ( ch == 127 && ! echo )
            echo = _echo;

        if ( echo ) {
            TextList    & tlist = this->getTextList();
            HexString   & inl   = tlist.back();
            std::string & ln    = cinput->getLine();
            int  indx           = ln.length() - 1;

            if ( indx < 0 )
                indx = 0;
            if ( HexString::CharIsVisible(ch) ) {
                if ( ln.length() < (size_t) _rescnt ) {
                    inl.str().replace(indx, 1, 1, ch);
                } else if ( echo ) {  // catch = length and stop echo
                    inl.str().replace(indx, 1, 1, ch);
                    echo = false;
                }
            } else if ( ch == 127 ) {
                inl.str().assign(ln);
                int cnt = _rescnt - ln.length();
                inl.str().append(cnt, '_');
            }
        }

        this->redraw();
        ch = this->poll();
    }
    _result = cinput->getLine();

    this->hide();

    return ch;
}


/** Returns any string results related to dialog input */
std::string
HexDialog::getResult()
{
    return _result;
}

/** By default, the dialog will exit on the first character
  * input. By setting this option to false, the dialog will
  * persist for string input until a carriage return <CR> 
  * is encountered.
 **/
void
HexDialog::setCharOnly ( bool chin )
{
    _char = chin;
}

/**  Allows multi-character input to be echo'ed to the dialog.
  *  Note that this will add a line of height to the dialog 
  *  if the dialog is dynamically sized.
 **/
void
HexDialog::echoResults ( bool echo )
{
    _echo = echo;
}

/**  Sets a maximum number of characters to be accepted as input. */
void
HexDialog::setMaxInput ( int count )
{
    if ( count > 1 )
        _char = false;
    _rescnt = count;
}


/**  Internal function to initiate the dialog dimensions and 
  *  location if the dialog is dynamically sized.
 **/
void
HexDialog::initDialog()
{
    int h, w, y, x;
            
    h = this->getLineCount();
    w = this->getLongestLine();

    if ( _echo ) {
        HexString   hexstr;
        std::string & line = hexstr.str();
        if ( _rescnt == 0 )
            _rescnt = w;
        line.append(_rescnt, '_');
        hexstr.alignment = HEX_ALIGN_CENTER;
        this->addText(hexstr);
        h += 1;
    }
    
    if ( this->getDrawBorder() ) {
        h += 2;
        w += 2;
    }

    y = ((LINES - h) / 2) - 1;
    x = ((COLS  - w) / 2);

    if ( _dynsz ) {
        this->resize(h, w);
    	this->moveWindow(y, x);
    }

    return;
}

size_t
HexDialog::getLongestLine()
{
    TextList & tlist = this->getTextList();
    TextList::iterator  tIter;
    size_t len = 0;

    for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter )
    {
        HexString & line = *tIter;
        if ( line.length() < len )
            continue;
        len = line.length() + 1;
    }

    return len;
}

} // namespace


// _HEXES_HEXDIALOG_CPP_

