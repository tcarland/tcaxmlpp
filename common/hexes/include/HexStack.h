/**
  * @file HexStack.h
  *
  * Copyright (c) 2011 Timothy Charlton Arland
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of libhexes.
  *
  * libhexes is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as
  * published by the Free Software Foundation, either version 3 of
  * the License, or (at your option) any later version.
  *
  * libhexes is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with libhexes.
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#ifndef _HEXES_HEXSTACK_H_
#define _HEXES_HEXSTACK_H_


#include "HexPanel.h"


namespace hexes {



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


    void setPanelFactory ( HexPanelFactory * factory );


  protected:

    PanelStack          _stack;
    HexPanel *          _curPanel;
    HexPanelFactory *   _factory;

    bool                _winCmd;

};


}  // namespace

#endif  // _HEXES_HEXSTACK_H_

