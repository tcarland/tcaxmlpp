#ifndef _HEXES_HEXPANEL_H_
#define _HEXES_HEXPANEL_H_

#include <panel.h>

#include <string>


namespace hexes {


class HexWindow;
class HexOutputInterface;
class HexInputInterface;


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


    virtual int redraw();


    virtual int handleDisplay();
    virtual int handleInput ( int ch );


    void        setOutputHandler ( HexOutputInterface * output );
    void        setInputHandler  ( HexInputInterface  * input  );

    void        print ( const std::string & str );

    void        show();
    void        hide();
    void        erase();
    void        refresh();

    void        setTitle();
    void        setTopPanel();

    int         width();
    int         height();

    void        enableBorder ( bool border );
    void        enableScroll ( bool scroll );
    bool        scrollable() const;


  protected:

    void        initPanel();


  private:
   
    HexWindow *          _hwin;
    PANEL *              _panel;

    HexOutputInterface * _output;
    HexInputInterface  * _input;

    std::string          _title;

    int                  _height, _width;
    int                  _starty, _startx;
    int                  _maxLines;
    int                  _selected;
    bool                 _scrollable;
    bool                 _drawBorder;
};


}  // namespace

#endif // _HEXES_HEXPANEL_H_

