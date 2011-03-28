#ifndef _HEXES_HEXSTRING_H_
#define _HEXES_HEXSTRING_H_

#include <string> 


namespace hexes {

/**  The HexString class simply wraps a stdc++ string object
  *  associating some of our needed HexApp attributes such as
  *  color and attributes from curses, alignment and wrap
  *  functionality used by HexPanel and others.
  *
  *  We allow this class to be extended to allow for making
  *  custom string behavior (eg. a TextField for use in forms)
 */
class HexString {
  public:
    typedef std::string::iterator  iterator;
    typedef std::string::size_type size_type;

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


    virtual void  operator=   ( const HexString   & hexstr );
    virtual void  operator=   ( const std::string & str );
    virtual bool  operator==  ( const HexString   & hexstr );


    virtual HexString& append ( const std::string & str );
    virtual HexString& append ( const HexString & hexstr );
    virtual HexString& append ( size_type count, char ch );
    virtual HexString& assign ( const std::string & str );
    virtual HexString& assign ( const HexString   & str );
    virtual HexString& assign ( size_type count, char ch );
    virtual HexString& erase  ( size_type from, size_type to = std::string::npos );
    virtual iterator   erase  ( iterator start, iterator end );
    virtual size_t     length() const;
    virtual bool       empty()  const;


    std::string&       str();
    const std::string& str()    const;

    /* some other obvious accessor's despite our attributes being public */
    int                getColor()      const { return color; }
    int                getAttributes() const { return attributes; }
    int                getAlignment()  const { return alignment; }
    int                getWrap()       const { return wrap; }

  public:

    static bool        CharIsVisible ( char ch );


  public:

    int          color;
    int          attributes;
    int          alignment;
    bool         wrap;

  //protected:

    std::string  _str;
};

} // namespace

#endif // _HEXES_HEXSTRING_H_

