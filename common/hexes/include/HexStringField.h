#ifndef _HEXSTRINGFIELD_H_
#define _HEXSTRINGFIELD_H_

#include "HexString.h"

namespace hexes {


/**  A custom HexString class that creates an entry field for a
  *  string and displays '_' as the field for the given length.
  *
  *  TODO:  Allow field to have title like USER: ______
  *         support scrolling within the field for longer strings.
 */
class HexStringField : public HexString {

  public:

    HexStringField ( size_t length = 0 );
    HexStringField ( const HexString & str, size_t length = 0 );
    HexStringField ( const std::string & str, size_t length = 0 );
    virtual ~HexStringField();

    virtual void  operator=   ( const HexString   & hexstr );
    virtual void  operator=   ( const std::string & str );

    virtual HexString& append ( const std::string & str );
    virtual HexString& append ( const HexString & hexstr );
    virtual HexString& append ( size_type count, char ch );

    virtual HexString& assign ( const std::string & str );
    virtual HexString& assign ( const HexString   & str );
    virtual HexString& assign ( size_type count, char ch );

    virtual HexString& erase  ( size_type from, size_type to = std::string::npos );
    virtual iterator   erase  ( iterator start, iterator end );

    virtual size_t     length() const;


  protected:

    size_t  _maxLen;
    size_t  _curLen;
    
};

} // namespace

#endif  // _HEXSTRINGFIELD_H_

