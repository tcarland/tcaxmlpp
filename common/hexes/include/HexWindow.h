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


    int            print ( const std::string & str );
    int            print ( const char ch );
    int            echo  ( const char ch );

    void           setBorder ( bool show );
    void           drawBorder();
    void           clearBorder();
    void           erase();

    int            width();
    int            height();

    int            currentColumn();
    int            currentRow();
    int            curY();
    int            curX();
    Position       currentPosition();


  protected:

    int            wrap();

    static int     LastIndexOf  ( const std::string & str, 
                                  const std::string & match,
                                  size_t from );

    static WINDOW* CreateWindow ( int height, int width, 
                                  int starty, int startx);


  protected:

    WINDOW *    _win;

    int         _height, _width;
    int         _starty, _startx;
    bool        _border;
    bool        _wrap;

};

} // namespace

#endif  // _HEXES_WINDOW_H_

