/**
  * @file LineOutputHandler.h
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
#ifndef _HEXES_LINEOUTPUTHANDLER_H_
#define _HEXES_LINEOUTPUTHANDLER_H_

#include "HexOutputInterface.hpp"


namespace hexes {

class HexPanel;


/**  A specialized class for handling output as lines. 
  *  Basically, our output is considerate of newlines and line
  *  lengths using a panel's given 'width' parameter for 
  *  ensuring proper line wrap.
 */
class LineOutputHandler : public HexOutputInterface {
  public:

    LineOutputHandler ( bool newline = true );
    virtual ~LineOutputHandler() {}

    virtual int handleOutput ( HexPanel * panel );

    bool  newline() const;
    void  setNewline ( bool newline );

  private:

    bool  _newline;
};

} // namespace

#endif // _HEXES_LINEOUTPUTHANDLER_H_

