/**
  * @file HexAttributes.h
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
#ifndef _HEXES_HEXATTRIBUTES_HPP_
#define _HEXES_HEXATTRIBUTES_HPP_


namespace hexes {


enum HexColorPair {
    HEX_DEFAULT_PAIR  = 0,
    HEX_RED_BLACK     = 1,
    HEX_GREEN_BLACK   = 2,
    HEX_YELLOW_BLACK  = 3,
    HEX_BLUE_BLACK    = 4,
    HEX_MAGENTA_BLACK = 5,
    HEX_CYAN_BLACK    = 6,
    HEX_WHITE_BLACK   = 7
};

enum HexColor {
    HEX_BLACK   = 0,
    HEX_RED     = 1,
    HEX_GREEN   = 2,
    HEX_YELLOW  = 3,
    HEX_BLUE    = 4,
    HEX_MAGENTA = 5,
    HEX_CYAN    = 6,
    HEX_WHITE   = 7
};

enum HexAttribute {
    HEX_NORMAL   = A_NORMAL,
    HEX_STANDOUT = A_STANDOUT,
    HEX_LINE     = A_UNDERLINE,
    HEX_REVERSE  = A_REVERSE,
    HEX_BLINK    = A_BLINK,
    HEX_DIM      = A_DIM,
    HEX_BOLD     = A_BOLD,
    HEX_PROTECT  = A_PROTECT,
    HEX_INVIS    = A_INVIS
};

enum HexAlignment {
    HEX_ALIGN_LEFT   = 0,
    HEX_ALIGN_RIGHT  = 1,
    HEX_ALIGN_CENTER = 2
};

enum HexCursor {
    HEX_CURSOR_INVIS  = 0,
    HEX_CURSOR_NORMAL = 1,
    HEX_CURSOR_BOLD   = 2
};

// some input values for convenience
enum HexInputKey {
    HEX_KEY_NONE = 0,
    HEX_KEY_WINDOW = 23
};

} // namespace

#endif // _HEXES_HEXATTRIBUTES_HPP_

