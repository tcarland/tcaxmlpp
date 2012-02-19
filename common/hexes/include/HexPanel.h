/**
  * @file HexPanel.h
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
#ifndef _HEXES_HEXPANEL_H_
#define _HEXES_HEXPANEL_H_

#include <string>
#include <list>

#include <panel.h>
#include <HexApp.h>


namespace hexes {

class HexApp;
class HexPanel;
class HexWindow;
class HexString;
class HexOutputInterface;
class HexInputInterface;


#define DEF_SCRLBK_SIZE  100
#define MAX_SCRLBK_SIZE  65535

typedef std::list<HexString>      HexStringList;
typedef hexes::HexStringList      TextList;



class HexPanelFactory {
  public:
    HexPanelFactory ( HexPanel * parent );
    virtual ~HexPanelFactory() {}

    virtual HexPanel* operator() ( int panelId, const std::string & title = "" );

  protected:
    HexPanel * _parent;
};




class HexPanel {

  public:

    HexPanel ( const std::string  & title = "" );

    HexPanel ( const std::string  & title,
               int   height, int    width,
               int   starty, int    startx );
    
    HexPanel ( const std::string  & title,
               HexOutputInterface * output,
               HexInputInterface  * input,
               int   height, int    width,
               int   starty, int    startx );

    virtual ~HexPanel();


    virtual int         redraw();
    virtual int         poll();
    virtual void        resize ( int height, int width );


    const std::string&  getPanelName() const;
    const std::string&  getName()      const { return this->getPanelName(); }
    void                setPanelId   ( int panelId );
    int                 getPanelId()   const;


    void                setOutputHandler ( HexOutputInterface * output );
    void                setInputHandler  ( HexInputInterface  * input  );
    HexOutputInterface* getOutputHandler();
    HexInputInterface*  getInputHandler();


    void                show();
    void                hide();
    void                erase();
    void                refresh();

    void                timeout      ( int delay_ms );
    
    int                 wrap();
    int                 move         ( int y, int x );
    int                 move         ( HexPosition & p );
    int                 moveWindow   ( int starty, int startx );

    int                 width();
    int                 height();
    int                 curY();
    int                 curX();
    int                 maxY();
    int                 maxX();
    int                 startY();
    int                 startX();

    void                addTextLine  ( const HexString   & hexstr ) { return this->addText(hexstr); }
    void                addTextLine  ( const std::string & str )    { return this->addText(str); }

    void                addText      ( const HexString   & hexstr );
    void                addText      ( const std::string & str );
    void                addText      ( const std::string & str, 
                                       int   color,
                                       int   attr = HEX_NORMAL );
    void                setText      ( const HexString   & hexstr );
    void                setText      ( const std::string & str );
    void                setText      ( const std::string & str,
                                       int   color,
                                       int   attr = HEX_NORMAL );
    void                clearText();
    void                clear();

    void                setTextList  ( TextList & textlist );
    TextList&           getTextList();
    int                 getLineCount();

    int                 print       ( HexString   & hexstr,
                                      HexPosition & pos );
    int                 print       ( const std::string & str,
                                      HexPosition       & pos,
                                      int  color,
                                      int  attr,
                                      bool wrap = false );
    int                 print       ( const std::string & str,
                                      bool wrap = false );
    int                 print       ( const char ch );
    int                 echo        ( const char ch );

    void                setWindowTitle ( HexString & hexstr );
    void                setWindowTitle ( const std::string & title, 
                                         int color, int attr = HEX_NORMAL );
    HexString&          getWindowTitle();

    void                setFocus();
    void                unsetFocus();

    int                 getTextColor();
    void                setTextColor   ( int colorIndex );

    int                 getBorderColor();
    void                setBorderColor ( int colorIndex );
    int                 getBorderActiveColor();
    void                setBorderActiveColor ( int colorIndex );

    int                 getBorderAttributes();
    void                setBorderAttributes  ( int attr );

    void                setAttribute   ( int attr );
    void                unsetAttribute ( int attr );

    void                setDrawBorder  ( bool border );
    bool                getDrawBorder() const;
    void                setDrawTitle   ( bool title );
    bool                getDrawTitle() const;

    bool                enableScroll   ( bool scroll,
                                         int  lines  = DEF_SCRLBK_SIZE );
    bool                setMaxLines    ( int  lines );
    void                scrollLine();

    int                 scrollTo();
    void                scrollUp();
    void                scrollDown();

    bool                scrollable()   const;


  protected:

    void                initPanel();


  private:
   
    HexWindow *          _hwin;
    PANEL *              _panel;

    HexOutputInterface * _output;
    HexInputInterface  * _input;

    std::string          _title;
    HexString            _winTitle;
    TextList             _textlist;

    int                  _panelId;

    int                  _selected;
    int                  _maxLines;
    int                  _scrollTo;

    int                  _txtColor, _bdrColor, _bfgColor;
    int                  _bdrAttr;

    bool                 _scrollable;
    bool                 _drawBorder, _drawTitle;
    bool                 _focus;
};


}  // namespace

#endif // _HEXES_HEXPANEL_H_

