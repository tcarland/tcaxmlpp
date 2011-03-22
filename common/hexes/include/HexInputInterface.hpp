#ifndef _HEXES_HEXINPUTINTERFACE_HPP_
#define _HEXES_HEXINPUTINTERFACE_HPP_

namespace hexes {

class HexPanel;


class HexInputInterface {
  public:
    virtual ~HexInputInterface() {}


    virtual int handleInput ( HexPanel * panel, int ch ) = 0;

};


} // namespace

#endif // _HEXES_HEXINPUTINTERFACE_HPP_

