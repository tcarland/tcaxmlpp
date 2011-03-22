#ifndef _HEXES_HEXATTRIBUTES_HPP_
#define _HEXES_HEXATTRIBUTES_HPP_


namespace hexes {


enum HexColorPair {
    HEX_WHITE_BLACK   = 0,
    HEX_RED_BLACK     = 1,
    HEX_GREEN_BLACK   = 2,
    HEX_YELLOW_BLACK  = 3,
    HEX_BLUE_BLACK    = 4,
    HEX_MAGENTA_BLACK = 5,
    HEX_CYAN_BLACK    = 6,
};

enum HexColor {
    HEX_BLACK,
    HEX_RED,
    HEX_GREEN,
    HEX_YELLOW,
    HEX_BLUE,
    HEX_MAGENTA,
    HEX_CYAN,
    HEX_WHITE
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

enum HexJustify {
    HEX_LEFT,
    HEX_RIGHT,
    HEX_CENTER
};


} // namespace

#endif // _HEXES_HEXATTRIBUTES_HPP_

