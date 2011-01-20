#ifndef _HEXES_LINEINPUTHANDLER_H_
#define _HEXES_LINEINPUTHANDLER_H_

#include <string>

#include "HexInputInterface.hpp"

namespace hexes {


class LineInputHandler : public HexInputInterface {

  public:

    LineInputHandler();

    virtual ~LineInputHandler();

    virtual int handleInput ( HexPanel * p, int ch );

    std::string getLine();
    bool        isReady();

  protected:

    std::string  _line;
    bool         _isReady;
};

    
} // namespace

#endif // _HEXES_LINEINPUTHANDLER_H_

