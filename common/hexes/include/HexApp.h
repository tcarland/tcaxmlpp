#ifndef _HEXES_HEXAPP_H_
#define _HEXES_HEXAPP_H_

#include <string>
#include <list>
#include <vector>
#include <map>

extern "C" {
#include <ncurses.h>
}

// functions timeout, erase, clear, etc., are macros that cause
// namespace collisions so we remap them. we also use the equivalent
// wtimeout/werase functions instead.
#ifdef timeout
#define curs_timeout timeout
#undef timeout
#endif
#ifdef erase
#define curs_erase erase
#undef erase
#endif


#include "HexAttributes.hpp"
#include "HexPosition.hpp"
#include "HexString.h"
#include "HexWindow.h"
#include "HexPanel.h"
#include "HexInputInterface.hpp"
#include "HexOutputInterface.hpp"
#include "LineInputHandler.h"
#include "LineOutputHandler.h"


namespace hexes {


#define LIBHEXES_VERSION "v0.105"


class HexPanel;


/**  Provides the frontend to a 'Hexes' application. HexApp is the
  * entry point to wrapping the libncurses/libpanel libraries for
  * the spawning and control of windows/panels. While the base
  * HexWindow and HexPanel objects can be used directly, if
  * desired, the HexApp class provides a convenient mechanism
  * for tracking and managing panels.
  *
 **/
class HexApp {

    typedef std::map<std::string, HexPanel*>    PanelMap;
    typedef std::vector<HexPanel*>              PanelStack;
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

    bool         addPanel       ( HexPanel * panel );
    HexPanel*    getPanel       ( const std::string & title = "" );
    HexPanel*    removePanel    ( const std::string & title );
    bool         destroyPanel   ( const std::string & title );
    void         destroyPanels();

    void         setFocus       ( HexPanel * panel );
    void         setFocusNext();
    void         setFocusPrev();

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
    int          print          ( int y, int x, 
                                  const std::string & str,
                                  int color, int attr );

  protected:


    static void         InitCurses ( bool termRaw, bool echo );
    static void         InitColors();
    static std::string  Version();


  private:

    static bool         _NCURSES_INIT;
    
    PanelMap            _panels;
    PanelStack          _pstack;
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
