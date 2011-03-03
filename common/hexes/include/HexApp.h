#ifndef _HEXES_HEXAPP_H_
#define _HEXES_HEXAPP_H_

#include <string>
#include <list>
#include <vector>
#include <map>

extern "C" {
#include <ncurses.h>
}

// functions timout(erase/clear) are macros that cause namespace
// collisions so we undef as needed.
// use the equivelant wtimeout & werase versions instead.
#ifdef timeout
#define curs_timeout timeout
#undef timeout
#endif
#ifdef erase
#define curs_erase erase
#undef erase
#endif


#include "HexPosition.hpp"
#include "HexString.hpp"
#include "HexWindow.h"
#include "HexPanel.h"
#include "HexInputInterface.hpp"
#include "HexOutputInterface.hpp"
#include "LineInputHandler.h"
#include "LineOutputHandler.h"


namespace hexes {


#define LIBHEXES_VERSION "v0.013a"


class HexPanel;


/**
  * Provides the frontend to a 'Hexes' application. HexApp is the
  * entrypoint to a ncurses/panel application and allows 
  * the spawning and control of windows/panels. 
  *
  * TODO: 
  *   1/13/11: still lots at this point.
  *   - HexWindow really needed? pull into HexPanel?
  *   - color schemes
  *   - window/text attributes
 **/
class HexApp {

    typedef std::vector<HexPanel*>              PanelStack;
    typedef std::map<std::string, HexPanel*>    PanelMap;
    typedef std::list<std::string>              StringList;

  public:

    HexApp();
    virtual ~HexApp();

  protected:

    virtual void resize() {}

  public:

    virtual void run() {}

    virtual int  draw();
    virtual int  poll();


    HexPanel*    createPanel    ( const std::string & title, 
                                  int height, int width, 
                                  int starty, int startx );
    void         setTopPanel    ( HexPanel * panel );
    bool         addPanel       ( HexPanel * panel );
    HexPanel*    getPanel       ( const std::string & title = "" );
    HexPanel*    removePanel    ( const std::string & title );
    bool         destroyPanel   ( const std::string & title );
    void         destroyPanels();

    void         timeout        ( int delay_ms );
    bool         hasColor() const;
    bool         resized();

    int          width();
    int          height();
    int          getMaxWidth();
    int          getMaxHeight();

    int          setCursor      ( int state );
    int          addColorPair   ( int fgcolor, int bgcolor );

    int          print          ( int y, int x, 
                                  const std::string & str );

  protected:


    static void         InitCurses ( bool termRaw, bool echo );
    static void         InitColors();
    static std::string  Version();


  private:

    static bool         _NCURSES_INIT;
    
    PanelMap            _panels;
    HexPanel *          _curPanel;

    int                 _row;
    int                 _col;
    int                 _colorIndex;
    bool                _echo;
    bool                _hasColor;

    std::string         _errstr;

};


}  // namespace

#endif  // _HEXES_HEXAPP_H_
