/**
  * @file HexStack.cpp
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
#define _HEXES_HEXSTACK_CPP_

#include "HexStack.h"


namespace hexes {



HexStack::HexStack ( const std::string & title )
    : HexPanel(title),
      _curPanel(NULL),
      _factory(new PanelFactory(this)),
      _winCmd(false)
{
}

HexStack::HexStack ( const std::string & title,
                     int   height, int   width,
                     int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _curPanel(NULL),
      _factory(new PanelFactory(this)),
      _winCmd(false)
{}


HexStack::~HexStack()
{
    if ( _factory )
        delete _factory;
}


int
HexStack::redraw()
{
    return HexPanel::redraw();
}

int
HexStack::poll()
{
    return HexPanel::poll();
}

void
HexStack:: resize ( int height, int width )
{
    return HexPanel::resize(height, width);
}

void
HexStack::setPanelFactory ( PanelFactory * factory )
{
    if ( this->_factory )
        delete _factory;
    _factory = factory;
}

}  // namespace


// _HEXES_HEXSTACK_CPP_
