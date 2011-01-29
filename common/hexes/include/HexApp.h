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
#undef erase
#endif


#include "HexPosition.hpp"
#include "HexWindow.h"
#include "HexPanel.h"
#include "HexInputInterface.hpp"
#include "HexOutputInterface.hpp"


namespace hexes {

#define LIBHEXES_VERSION "v0.003a"


class HexPanel;


/**
  * Frontend to a 'Hex' application. HexApp provides the 
  * entrypoint to a ncurses/panel application and allows 
  * the spawning and control of windows/panels. 
  *
  * TODO: 
  *   1/13/11: still everything at this point.
  *   - add panel stack handling.
  *   - we are managing the HexPanel ptr's; 
  *     should we use boost::shared_ptr<>?  
  *   - HexWindow really needed? pull into HexPanel?
  *   - color schemes
  *   - window/text attributes
  *   - rescale logic
 **/
class HexApp {

    typedef std::vector<HexPanel*>              PanelStack;
    typedef std::map<std::string, HexPanel*>    PanelMap;
    typedef std::list<std::string>              StringList;

  public:

    HexApp();
    virtual ~HexApp();

    virtual void run() {}

    virtual int  draw();
    virtual int  poll();


    HexPanel*    createPanel   ( const std::string & title, 
                                 int height, int width, 
                                 int starty, int startx );

    bool         addPanel      ( HexPanel * panel );
    HexPanel*    getPanel      ( const std::string & title );
    HexPanel*    removePanel   ( const std::string & title );
    bool         destroyPanel  ( const std::string & title );


    void         setTopPanel   ( HexPanel * panel );

    void         destroyPanels();
    bool         hasColor() const;

    int          width();
    int          height();
    int          getMaxWidth();
    int          getMaxHeight();

    int          print         ( int y, int x, 
                                 const std::string & str );

  protected:


    static void         InitCurses ( bool termRaw, bool echo );
    static std::string  Version();


  private:

    static bool         _CURSESINIT;
    
    PanelMap            _panels;
    StringList          _panel_list;
    HexPanel *          _curPanel;

    int                 _row;
    int                 _col;
    bool                _echo;
    bool                _hasColor;

    std::string         _errstr;

};


}  // namespace

#endif  // _HEXES_HEXAPP_H_
