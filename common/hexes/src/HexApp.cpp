#define HEXES_HEXAPP_CPP_

#include <ncurses.h>

#include "HexApp.h"


namespace hexes {


bool HexApp::_NCURSES_INIT = false;

//----------------------------------------------------------------//

HexApp::HexApp()
    : _curPanel(NULL),
      _colorIndex(0),
      _echo(false)
{
    this->InitCurses(false, _echo);
    _hasColor = ::has_colors();
    _col      = COLS;
    _row      = LINES;
}


HexApp::~HexApp()
{
    this->destroyPanels();
    nocbreak();
    endwin();
}

//----------------------------------------------------------------//

bool
HexApp::resized()
{
    bool resized = false;
    if ( _row != LINES || _col != COLS )
    {
        _col    = COLS;
        _row    = LINES;
        resized = true;
    }
    return resized;
}

int
HexApp::draw()
{
    int  result = 0;

    if ( this->resized() )
        this->resize();

    StringList  removes;

    PanelMap::iterator  pIter;
    for ( pIter = _panels.begin(); pIter != _panels.end(); ++pIter )
    {
        HexPanel * p = pIter->second;

        if ( p == NULL ) {
            removes.push_back(pIter->first);
            continue;
        }

        result = p->redraw();

        if ( result == 0 ) {
            p->erase();
            removes.push_back(pIter->first);
        }
    }

    StringList::iterator sIter;

    for ( sIter = removes.begin(); sIter != removes.end(); ++sIter ) {
        pIter = _panels.find(*sIter);
        if ( pIter == _panels.end() )
            continue;
        if ( pIter->second )
            delete pIter->second;
        _panels.erase(pIter);
    }
    removes.clear();
    
    //set current panel
    //reset cursor pos.
    if ( _curPanel ) {
        this->setFocus(_curPanel);
        _curPanel->move(_curPanel->curY(), _curPanel->curX());
    }
    
    ::update_panels();
    ::doupdate();
    ::refresh();

    return 1;
}

//----------------------------------------------------------------//

int
HexApp::poll()
{
    int r = ERR;
    
    if ( _curPanel != NULL )
        r = _curPanel->poll();

    if ( r == KEY_RESIZE && this->resized() )
        this->resize();

    if ( r != ERR )
        return r;

    PanelMap::iterator pIter;
    for ( pIter = _panels.begin(); pIter != _panels.end(); ++pIter )
    {
        r = pIter->second->poll();
        if ( r != ERR )
            break;
    }

    if ( r == KEY_RESIZE && this->resized() )
        this->resize();
    
    return r;
}

void
HexApp::timeout ( int delay_ms )
{
    if ( _curPanel != NULL )
        _curPanel->timeout(delay_ms);
}

//----------------------------------------------------------------//

void
HexApp::setFocus ( HexPanel * panel )
{
    if ( panel == NULL )
        return;

    panel->setTopPanel();

    _curPanel = panel;
}

void
HexApp::setFocusNext()
{
    if ( _curPanel == NULL )
    {
        if ( _pstack.empty() )
            return;
        _curPanel = _pstack.front();
    }

    if ( _curPanel && _pstack.size() > 1 )
    {
        int id = _curPanel->getPanelId() + 1;
        if ( (size_t) id >= _pstack.size() )
            id = 0;
        this->setFocus(_pstack.at(id));
    }
    return;
}

void
HexApp::setFocusPrev()
{
    if ( _curPanel == NULL )
    {
        if ( _pstack.empty() )
            return;
        _curPanel = _pstack.front();
    }

    if ( _curPanel && _pstack.size() > 1 )
    {
        int id = _curPanel->getPanelId() - 1;
        if ( id < 0 )
            id = 0;
        this->setFocus(_pstack.at(id));
    }
    return;
}

//----------------------------------------------------------------//

HexPanel*
HexApp::createPanel ( const std::string & title, 
                      int height, int width, 
                      int starty, int startx )
{
    HexPanel * p = NULL;
    PanelMap::iterator pIter;

    pIter = _panels.find(title);
    if ( pIter != _panels.end() ) {
        return p;
    }

    p = new HexPanel(title, height, width, starty, startx);
    p->setPanelId(_pstack.size());

    _panels[title] = p;
    _pstack.push_back(p);

    if ( _curPanel == NULL )
        this->setFocus(p);

    return p;
}

//----------------------------------------------------------------//

/**  Adds the HexPanel to this HexApp instance. By doing so, 
  *  the HexApp instance will 'own' the panel, calling the 
  *  proper I/O events on redraw and poll, and freeing the 
  *  object on destruction.
 **/
bool
HexApp::addPanel ( HexPanel * panel )
{
    if ( panel == NULL )
        return false;

    PanelMap::iterator pIter;

    pIter = _panels.find(panel->getPanelName());

    if ( pIter != _panels.end() )
        return false;
    
    panel->setPanelId(_pstack.size());
    _panels[panel->getPanelName()] = panel;
    _pstack.push_back(panel);

    if ( _curPanel == NULL )
        this->setFocus(panel);

    return true;
}


HexPanel*
HexApp::getPanel ( const std::string & title )
{
    HexPanel * panel = NULL;
    PanelMap::iterator  pIter;

    if ( title.empty() && _curPanel != NULL )
        panel = _curPanel;

    pIter = _panels.find(title);

    if ( pIter != _panels.end() )
        panel = pIter->second;

    return panel;
}


/** Removes the panel identified by @param title and returns 
  * the pointer to the corresponding HexPanel. HexApp will no 
  * longer track the panel object and it will not be free'd by
  * the HexApp instance. Returns NULL if no such panel exists.
 **/
HexPanel*
HexApp::removePanel ( const std::string & title )
{
    HexPanel * panel = NULL;
    PanelMap::iterator pIter;
    int id;

    pIter = _panels.find(title);
    if ( pIter == _panels.end() )
        return panel;

    panel = pIter->second;
    id    = panel->getPanelId();

    _panels.erase(pIter);
    _pstack.erase(_pstack.begin() + id);

    if ( id > 0 )
        id--;

    PanelStack::iterator sIter;
    for ( sIter = _pstack.begin()+id; sIter != _pstack.end(); ++sIter, ++id )
        (*sIter)->setPanelId(id);

    if ( _curPanel == panel )
        _curPanel = _pstack.front();

    return panel;
}

/**  Removes and destroys(free) the HexPanel instance identified 
  *  by @param title. Return false if no such panel exists.
 **/
bool
HexApp::destroyPanel ( const std::string & title )
{
    HexPanel * panel = NULL;
    PanelMap::iterator pIter;
    int id;

    pIter = _panels.find(title);
    if ( pIter == _panels.end() )
        return false;

    panel = pIter->second;
    id    = panel->getPanelId();

    _panels.erase(pIter);
    _pstack.erase(_pstack.begin() + id);

    if ( id > 0 )
        id--;

    PanelStack::iterator sIter;
    for ( sIter = _pstack.begin()+id; sIter != _pstack.end(); ++sIter, ++id )
        (*sIter)->setPanelId(id);

    if ( _curPanel == panel )
        _curPanel = _pstack.front();

    delete panel;

    return true;
}

/** Destroys all panels tracked by this instance of HexApp */
void
HexApp::destroyPanels()
{
    PanelMap::iterator pIter;

    for ( pIter = _panels.begin(); pIter != _panels.end(); ++pIter )
    {
        if ( pIter->second != NULL )
            delete pIter->second;
    }
    _panels.clear();
    _pstack.clear();
    _curPanel = NULL;

    return;
}

//----------------------------------------------------------------//

int
HexApp::getMaxWidth()
{
    return this->width();
}

int
HexApp::width()
{
    return _col;
}

int
HexApp::height()
{
    return _row;
}

int
HexApp::getMaxHeight()
{
    return this->height();
}

/** Sets the cursor state to invisible, normal or very visible
  * for state equal to 0, 1, or 2 respectively.
 **/
int
HexApp::setCursor ( int state )
{
    return ::curs_set(state);
}

//----------------------------------------------------------------//

bool
HexApp::hasColor() const
{
    return _hasColor;
}

int
HexApp::addColorPair ( int fgcolor, int bgcolor )
{
    if ( _colorIndex == 0 )
        _colorIndex = HEX_WHITE_BLACK;

    _colorIndex++;
    ::init_pair(_colorIndex, fgcolor, bgcolor);

    return _colorIndex;
}

//----------------------------------------------------------------//

int
HexApp::print ( int y, int x, const std::string & str )
{
    return(mvwaddstr(stdscr, y, x, str.c_str()));
}

int
HexApp::print ( int y, int x, const std::string & str, int color, int attr )
{
    int r = 0;
    ::wattron(stdscr, COLOR_PAIR(color)|attr);
    r = mvwaddstr(stdscr, y, x, str.c_str());
    ::wattroff(stdscr, COLOR_PAIR(color)|attr);
    return r;
}


//----------------------------------------------------------------//

void
HexApp::InitCurses ( bool termRaw, bool echo )
{
    if ( HexApp::_NCURSES_INIT )
        return;

    ::initscr();

    if ( termRaw )
        ::raw();
    else
        ::cbreak();

    if ( ! echo )
        ::noecho();

    ::nonl();
    ::keypad(stdscr, TRUE);

    HexApp::InitColors();
    HexApp::_NCURSES_INIT = true;

    return;
}

void
HexApp::InitColors()
{
    ::start_color();
    ::init_pair(HEX_RED_BLACK, HEX_RED, HEX_BLACK);
    ::init_pair(HEX_GREEN_BLACK, HEX_GREEN, HEX_BLACK);
    ::init_pair(HEX_YELLOW_BLACK, HEX_YELLOW, HEX_BLACK);
    ::init_pair(HEX_BLUE_BLACK, HEX_BLUE, HEX_BLACK);
    ::init_pair(HEX_MAGENTA_BLACK, HEX_MAGENTA, HEX_BLACK);
    ::init_pair(HEX_CYAN_BLACK, HEX_CYAN, HEX_BLACK);
}

//----------------------------------------------------------------//

std::string
HexApp::Version()
{
    std::string ver = LIBHEXES_VERSION;
    return ver;
}


}  // namespace

// HEXES_HEXAPP_CPP_

