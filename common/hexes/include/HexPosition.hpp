/**
  * @file HexPosition.hpp
  *
  * Copyright (c) 2011 Timothy Charlton Arland
  * @author tcarland@gmail.com
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
#ifndef _HEXES_HEXPOSITION_HPP_
#define _HEXES_HEXPOSITION_HPP_


namespace hexes {


/**  Represents a pair of X, Y screen coordinates */
typedef struct HexPosition {
    int  row;  // y
    int  col;  // x

    HexPosition() 
        : row(1), col(1) 
    {}

    HexPosition ( int y, int x ) 
        : row(y), col(x) 
    {}

    int x() { return col; }
    int y() { return row; }

} hexPos_t;


} // namespace

#endif  // _HEXES_HEXPOSITION_HPP_

