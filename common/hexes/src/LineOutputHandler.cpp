/**
  * @file HexOutputHandler.cpp
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
#define _HEXES_LINEOUTPUTHANDLER_CPP_

#include "LineOutputHandler.h"
#include "HexPanel.h"


namespace hexes {


LineOutputHandler::LineOutputHandler ( bool newline )
    : _newline(newline)
{}


int
LineOutputHandler::handleOutput ( HexPanel * panel )
{
    TextList::iterator tIter;
    TextList & tlist = panel->getTextList();

    int ht = panel->height();
    int wd = panel->width();
    int st = panel->scrollTo();
    int ln = 1;
    int bw = 2;  //border width

    if ( panel->getDrawBorder() ) {
        ht -= bw;
        wd -= bw;
        panel->move(1, 1);
    }

    if ( st > 0 && tlist.size() > (size_t) ht )
        st = tlist.size() - st;
    else
        st = 0;

    for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter, ++ln )
    {
        HexString & line = *tIter;

        if ( ln > ht && panel->scrollable() )
            panel->scrollLine();

        panel->setAttribute(COLOR_PAIR(line.color));
        if ( line.attributes > HEX_NORMAL )
            panel->setAttribute(line.attributes);

        if ( line.alignment == HEX_ALIGN_CENTER ) {
            int stfrom = (wd - line.length()) / 2;
            panel->move(panel->curY(), stfrom);
        } else if ( line.alignment == HEX_ALIGN_RIGHT ) {
            int stfrom = wd - line.length() - 1;
            panel->move(panel->curY(), stfrom);
        }

        panel->print(line.str());

        panel->unsetAttribute(COLOR_PAIR(line.color));
        if ( line.attributes > HEX_NORMAL )
            panel->unsetAttribute(line.attributes);

        if ( ln < ht && (size_t) ln != tlist.size() && _newline )
            panel->wrap();
        else
            panel->move(panel->curY(), 1); // err..if ! newline?

        if ( st > 0 && st == ln )
            break;
    }

    return 1;
}


bool
LineOutputHandler::newline() const
{
    return this->_newline;
}


void
LineOutputHandler::setNewline ( bool newline )
{
    _newline = newline;
}

} // namespace

