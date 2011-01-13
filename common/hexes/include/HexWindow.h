#ifndef _HEXES_WINDOW_H_
#define _HEXES_WINDOW_H_

#include <string>

#include "HexApp.h"

namespace hexes {


/* Not sure I should bother making this separate object for wrapping
 * a curses 'WINDOW'. Why not always use a panel for any and all windows?
 * I see no disadvantages to doing so, since panels simply allow for 
 * window manipulation.
 * Initial logic was building a library that might be functional without
 * libpanel.
 *
 */
class HexWindow {

    friend class HexPanel;

    typedef struct Position {
        int row, col;
        Position() : row(1), col(1) 
        {}
    } pos;

  public:

    HexWindow();
    HexWindow ( int height, int width,
                bool border = true );
    HexWindow ( int height, int width,
                int starty, int startx,
                bool border = true );

    virtual ~HexWindow();


    void           print ( const std::string & str );

    void           setBorder ( bool show );
    void           drawBorder();
    void           clearBorder();
    void           erase();

    int            width();
    int            height();


  protected:


    static WINDOW* CreateWindow ( int height, int width, 
                                  int starty, int startx);


  protected:

    WINDOW *    _win;

    int         _height, _width;
    int         _starty, _startx;
    bool        _border;

    Position    _curP;

};

} // namespace

#endif  // _HEXES_WINDOW_H_

