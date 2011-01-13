#ifndef _HEXES_WINDOW_H_
#define _HEXES_WINDOW_H_

#include <string>

#include "HexApp.h"

namespace hexes {



class HexWindow {

  friend class HexPanel;

  public:

    HexWindow();
    HexWindow ( int height, int width );
    HexWindow ( int height, int width,
                int starty, int startx );

    virtual ~HexWindow();


    void           print ( const std::string & str );

    WINDOW*        getWindow();

    void           drawBorder();
    void           clearBorder();
    void           erase();

  protected:


    static WINDOW* CreateWindow ( int height, int width, 
                                  int starty, int startx);


  protected:

    WINDOW *    _win;

    int         _height, _width;
    int         _starty, _startx;

};

} // namespace

#endif  // _HEXES_WINDOW_H_

