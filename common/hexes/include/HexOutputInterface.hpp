/**
  * @file HexOutputInterface.h
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
#ifndef _HEXES_HEXOUTPUTINTERFACE_HPP_
#define _HEXES_HEXOUTPUTINTERFACE_HPP_

namespace hexes {

class HexPanel;

/**  Defines the hexes interface for handling output from a curses  
  *  window. Triggered by a draw event from HexApp, the derived 
  *  method is generally called for each HexPanel of the 
  *  application to allow each panel and underlying window to 
  *  control its own output behavior.
 **/
class HexOutputInterface {
  public:
    virtual ~HexOutputInterface() {}

    virtual int handleOutput ( HexPanel * panel ) = 0;
};

} // namespace 

#endif // _HEXES_HEXOUTPUTINTERFACE_HPP_

