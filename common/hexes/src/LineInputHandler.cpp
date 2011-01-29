#define _HEXES_LINEINPUTHANDLER_CPP_


#include "LineInputHandler.h"
#include "HexApp.h"


namespace hexes {


LineInputHandler::LineInputHandler()
    : _isReady(false)
{}


LineInputHandler::~LineInputHandler()
{}


int 
LineInputHandler::handleInput ( HexPanel * p, int ch )
{
    if ( _isReady ) {
        _line.clear();
        _isReady = false;
    }

    if ( ch == KEY_BACKSPACE || ch == 127 ) {
        int x = p->curX();
        p->move(p->curY(), x-1);
        p->print(' ');
        p->move(p->curY(), x-1);
        p->refresh();
        _line.erase(_line.size() - 1);
    } else if ( ch == KEY_UP ) {
        ;
    } else if ( ch == KEY_DOWN ) {
        ;
    } else if ( ch == KEY_RIGHT ) {
        ;
    } else if ( ch == KEY_LEFT ) {
        ;
    } else if ( ch == KEY_ENTER || ch == 13 ) {
        _isReady = true;
    } else {
        p->print(ch);
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


}  // namespace

// _HEXES_LINEINPUTHANDLER_CPP_

