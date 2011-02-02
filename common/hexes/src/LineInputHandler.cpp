#define _HEXES_LINEINPUTHANDLER_CPP_


#include "LineInputHandler.h"
#include "HexApp.h"


namespace hexes {


LineInputHandler::LineInputHandler ( int maxlength )
    : _maxlen(maxlength),
      _isReady(false),
      _echo(true),
      _lines(HEXES_DEFBUFLEN)
{}


LineInputHandler::~LineInputHandler()
{}


int 
LineInputHandler::handleInput ( HexPanel * p, int ch )
{
    if ( _isReady ) {
        _history.push_back(_line);
        while ( _history.size() > _lines )
            _history.erase(_history.begin());
        _line.clear();
        _isReady = false;
    }

    if ( ch == KEY_BACKSPACE || ch == 127 ) {
        if ( _line.size() > 0 ) {
            TextList    & txtlist = p->getTextList();
            std::string & curline = txtlist.front();
            curline.erase(curline.size() - 1);
	    _line.erase(_line.size() - 1);

	    int x = p->curX();
	    p->move(p->curY(), x-1);
	   // p->print(' ');
	    //p->move(p->curY(), x-1);
	    //p->refresh();
	    //p->redraw();
        }
    } else if ( ch == KEY_UP ) {
        ; // cmd history
    } else if ( ch == KEY_DOWN ) {
        ;
    } else if ( ch == KEY_RIGHT ) {
        ;
    } else if ( ch == KEY_LEFT ) {
        ;
    } else if ( ch == KEY_ENTER || ch == 13 ) {
        _isReady = true;
    } else if ( ch >= 32 && ch < 128 ) {
        if ( _echo ) {
            TextList & txtlist = p->getTextList();
            if (txtlist.size() == 0 ) {
                std::string  f = "";
                f.append(1, ch);
                p->addText(f);
            } else {
	        std::string & curline = txtlist.front();
	        curline.append(1, ch);
            }
        }
        _line.append(1, ch);
    }

    return ch;
}

std::string&
LineInputHandler::getLine()
{
    return _line;
}

bool
LineInputHandler::isReady()
{
    return _isReady;
}

bool
LineInputHandler::echo()
{
    return _echo;
}


void
LineInputHandler::setEcho ( bool echo )
{
    _echo = echo;
}

void
LineInputHandler::keepHistory ( int lines )
{
    _lines = lines;
}


}  // namespace

// _HEXES_LINEINPUTHANDLER_CPP_

