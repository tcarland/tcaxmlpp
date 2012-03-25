/**
  * @file HexDialog.h
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
#ifndef _HEXES_HEXDIALOG_H_
#define _HEXES_HEXDIALOG_H_

#include <string>

#include "HexPanel.h"


namespace hexes {



/** A derivative of HexPanel, the HexDialog class represents
  * a modal, pop-up dialog window.
 **/
class HexDialog : public HexPanel {

  public:

    HexDialog ( const std::string & title,
                const HexString   & dialog = HexString() );

    HexDialog ( const std::string & title,
                const HexString   & dialog,
                int   height, int   width,
                int   starty, int   startx );

    virtual ~HexDialog();


    virtual int  showDialog();


    std::string  getResult();

    void         setCharOnly ( bool chin );
    void         echoResults ( bool echo, char echochar = 0 );
    void         setMaxInput ( int  count );


  protected:

    void         initDialog();
    size_t       getLongestLine();


  private:

    std::string  _result;

    char         _echochar;
    int          _rescnt;
    bool         _dynsz;
    bool         _char;
    bool         _echo;

};

} // namespace 

#endif // _HEXES_HEXDIALOG_H_
