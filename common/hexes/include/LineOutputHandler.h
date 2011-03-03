#ifndef _HEXES_LINEOUTPUTHANDLER_H_
#define _HEXES_LINEOUTPUTHANDLER_H_

#include "HexOutputInterface.hpp"


namespace hexes {

class HexPanel;


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

