#ifndef _HEXES_HEXOUTPUTINTERFACE_HPP_
#define _HEXES_HEXOUTPUTINTERFACE_HPP_

namespace hexes {

class HexPanel;

class HexOutputInterface {
  public:
    virtual ~HexOutputInterface() {}

    virtual int handleOutput ( HexPanel * panel ) = 0;
};

} // namespace 

#endif // _HEXES_HEXOUTPUTINTERFACE_HPP_

