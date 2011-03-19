#ifndef _HEXES_HEXSTRING_HPP_
#define _HEXES_HEXSTRING_HPP_

#include <string> 


namespace hexes {


struct HexString {

    std::string  str;
    int          alignment;
    int          attributes;
    int          color;
    bool         wrap;

    HexString()
      : alignment(0),
        attributes(0),
        color(0),
        wrap(true)
    {}

    HexString ( const std::string & str_, 
                bool  wrap_ = true ) 
      : str(str_),
        alignment(0),
        attributes(0),
        color(0),
        wrap(wrap_)
    {}
};

} // namespace

#endif // _HEXES_HEXSTRING_HPP_

