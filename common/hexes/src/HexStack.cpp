#define _HEXES_HEXSTACK_CPP_

#include "HexStack.h"


namespace hexes {

PanelFactory::PanelFactory ( HexPanel * parent )
    : _parent(parent)
{}

HexPanel*
PanelFactory::operator() ( int panelId, const std::string & title )
{
    return NULL;
}


HexStack::HexStack ( const std::string & title )
    : HexPanel(title),
      _curPanel(NULL),
      _factory(new PanelFactory(this)),
      _winCmd(false)
{
}

HexStack::HexStack ( const std::string & title,
                     int   height, int   width,
                     int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _curPanel(NULL),
      _factory(new PanelFactory(this)),
      _winCmd(false)
{}


HexStack::~HexStack()
{
    if ( _factory )
        delete _factory;
}


int
HexStack::redraw()
{
    return HexPanel::redraw();
}

int
HexStack::poll()
{
    return HexPanel::poll();
}

void
HexStack:: resize ( int height, int width )
{
    return HexPanel::resize(height, width);
}

void
HexStack::setPanelFactory ( PanelFactory * factory )
{
    if ( this->_factory )
        delete _factory;
    _factory = factory;
}

}  // namespace


// _HEXES_HEXSTACK_CPP_
