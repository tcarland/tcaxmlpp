/**
  * @file HexInputInterface.h
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
#ifndef _HEXES_HEXINPUTINTERFACE_HPP_
#define _HEXES_HEXINPUTINTERFACE_HPP_

namespace hexes {

class HexPanel;

/**  Defines a simple interface for handling input data from 
  *  a hex application. Input on a panel will cause the 
  *  derived method to be called with the HexPanel that 
  *  caught the input, and the character given.
 **/
class HexInputInterface {
  public:
    virtual ~HexInputInterface() {}


    virtual int handleInput ( HexPanel * panel, int ch ) = 0;

};


} // namespace

#endif // _HEXES_HEXINPUTINTERFACE_HPP_

