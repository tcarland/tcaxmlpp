#ifndef _HEXSTRINGFIELD_H_
#define _HEXSTRINGFIELD_H_

#include "HexString.h"

namespace hexes {


class HexStringField : public HexString {

  public:

    HexStringField ( size_t vis_length, size_t length = 0 );
    virtual ~HexStringField();

    virtual HexString& append ( const std::string & str );
    virtual HexString& append ( const HexString & hexstr );
    virtual HexString& append ( size_t count, char ch );

    virtual HexString& assign ( const std::string & str );
    virtual HexString& assign ( const HexString   & str );
    virtual HexString& assign ( size_t count, char ch );

  private:

    size_t  _maxLen;
    size_t  _visLen;
    
};

} // namespace

#endif  // _HEXSTRINGFIELD_H_

