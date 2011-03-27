#ifndef _HEXES_HEXSTACK_H_
#define _HEXES_HEXSTACK_H_


#include "HexPanel.h"


namespace hexes {


class PanelFactory {
    HexPanel * _parent;
  public:
    PanelFactory ( HexPanel * parent );
    virtual ~PanelFactory() {}
    virtual HexPanel* operator() ( int panelId, const std::string & title = "" );
};


class HexStack : public HexPanel {

  public:

    HexStack ( const std::string & title = "" );
    HexStack ( const std::string & title,
               int   height, int   width,
               int   starty, int   startx );

    virtual ~HexStack();

    
    virtual int  redraw();
    virtual int  poll();
    virtual void resize  ( int height, int width );

    void setPanelFactory ( PanelFactory * factory );


  protected:

    PanelStack          _stack;
    HexPanel *          _curPanel;
    PanelFactory *      _factory;

    bool                _winCmd;

};


}  // namespace

#endif  // _HEXES_HEXSTACK_H_

