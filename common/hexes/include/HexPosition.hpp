#ifndef _HEXES_HEXPOSITION_HPP_
#define _HEXES_HEXPOSITION_HPP_


namespace hexes {


typedef struct HexPosition {
    int  row;  // y
    int  col;  // x

    HexPosition() 
        : row(1), col(1) 
    {}

    HexPosition ( int y, int x ) 
        : row(y), col(x) 
    {}

} hexPos_t;


} // namespace

#endif  // _HEXES_HEXPOSITION_HPP_

