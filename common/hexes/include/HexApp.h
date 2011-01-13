#ifndef _HEXES_HEXAPP_H_
#define _HEXES_HEXAPP_H_

#include <string>
#include <list>
#include <vector>
#include <map>

#include <ncurses.h>

#include "HexWindow.h"
#include "HexPanel.h"


namespace hexes {

class HexPanel;


class HexApp {

    struct Position {
        int row;
        int col;
    };

    typedef std::vector<HexPanel*>              PanelStack;
    typedef std::map<std::string, HexPanel*>    PanelMap;
    typedef std::list<std::string>              StringList;

  public:

    HexApp();
    virtual ~HexApp();


    virtual void run() {}
    virtual int  draw();
    virtual int  rescale();


    HexPanel*    createPanel   ( const std::string & title, 
                                 int height, int width, 
                                 int starty, int startx );


    HexPanel*    getPanel      ( const std::string & title );
    HexPanel*    removePanel   ( const std::string & title );
    bool         destroyPanel  ( const std::string & title );

    void         setTopPanel   ( HexPanel * panel );


    void         destroyPanels();
    bool         hasColors() const;


  protected:



  private:

    static void InitCurses ( bool termRaw, bool echo );



  private:

    static bool         NCURSES_LIBINIT;
    
    PanelMap            _panels;

    int                 _row;
    int                 _col;
    bool                _echo;
    bool                _hasColors;

    std::string         _errstr;

};


}  // namespace

#endif  // _HEXES_HEXAPP_H_
