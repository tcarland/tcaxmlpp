#ifndef _HEXES_HEXSTRING_H_
#define _HEXES_HEXSTRING_H_

#include <string> 


namespace hexes {


class HexString {

  public:

    HexString();

    HexString ( const std::string & str,
                bool  wrap = true );

    HexString ( const std::string & str,
                int   color,
                int   attributes,
                bool  wrap  = true );

    HexString ( const HexString & hexstr );

    virtual ~HexString();


    void operator=  ( const HexString & hexstr );
    bool operator== ( const HexString & hexstr );

  public:

    std::string  str;
    int          color;
    int          attributes;
    int          alignment;
    bool         wrap;
};

} // namespace

#endif // _HEXES_HEXSTRING_H_

