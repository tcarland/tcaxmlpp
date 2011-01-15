#ifndef _HEXES_TESTHEXAPP_H_
#define _HEXES_TESTHEXAPP_H_

#include "HexApp.h"
#ifdef timeout
#undef timeout
#endif

namespace hexes {


class TestHexApp : public HexApp {

  public:

    virtual ~TestHexApp() {}

    virtual void run();

};


}  // namespace

#endif // _HEXES_TESTHEXAPP_H_

