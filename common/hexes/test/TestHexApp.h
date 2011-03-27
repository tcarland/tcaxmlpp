#ifndef _HEXES_TESTHEXAPP_H_
#define _HEXES_TESTHEXAPP_H_

#include "HexApp.h"
#ifdef timeout
#undef timeout
#endif

namespace hexes {


class TestHexApp : public HexApp {

  public:

    TestHexApp();
    virtual ~TestHexApp() {}

    virtual void run();

  protected:

    virtual void resize();
    void bomb();
    void help();


  private:

    HexPanel * mainPanel, *statPanel, *conPanel;

    int _conheight, _titleheight, _statheight, _mainheight;
};


}  // namespace

#endif // _HEXES_TESTHEXAPP_H_

