#ifndef _HEXES_LINEOUTPUTHANDLER_H_
#define _HEXES_LINEOUTPUTHANDLER_H_

#include "HexOutputInterface.hpp"


namespace hexes {

class HexPanel;


class LineOutputHandler : public HexOutputInterface {
  public:

    virtual ~LineOutputHandler() {}

    virtual int handleOutput ( HexPanel * panel );

};

} // namespace

#endif // _HEXES_LINEOUTPUTHANDLER_H_

