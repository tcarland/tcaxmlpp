#define _HEXES_LINEINPUTHANDLER_CPP_


#include "LineInputHandler.h"
#include "HexApp.h"


namespace hexes {


LineInputHandler::LineInputHandler ( int maxlength )
    : _maxlen(maxlength),
      _isReady(false),
      _echo(true),
      _parse(true),
      _lines(HEXES_DEFBUFLEN),
      _hindx(0)
{}


LineInputHandler::~LineInputHandler()
{}


int 
LineInputHandler::handleInput ( HexPanel * p, int ch )
{
    if ( ! _parse )
        return ch;

    if ( _isReady ) {
        if ( ! _line.empty() ) {
            _history.push_back(_line);
            while ( _history.size() > _lines )
                _history.erase(_history.begin());
            _line.clear();
        }
        _isReady = false;
        _hindx   = _history.size();
    }

    if ( ch == KEY_BACKSPACE || ch == 127 )
    {
        if ( _line.size() > 0 ) {
            TextList    & txtlist = p->getTextList();
            HexString   & curline = txtlist.back();
            curline.erase(curline.length() - 1);
	    _line.erase(_line.length() - 1);

	    int x = p->curX();
	    p->move(p->curY(), x-1);
        }
    }
    else if ( ch == KEY_UP )
    {
        if ( _history.empty() || _hindx == 0 )
            return ch;

        if ( _hindx == _history.size() && ! _line.empty() ) {
            _history.push_back(_line);
        }

        _hindx--;

        TextList  & txtlist = p->getTextList();
        HexString & curline = txtlist.back();
        if ( ! _prompt.empty() ) {
            curline.str().assign(_prompt);
        } else {
            curline.str() = "";
        }
        curline.str().append(_history.at(_hindx));
        _line.assign(_history.at(_hindx));
    }
    else if ( ch == KEY_DOWN )
    {
        _hindx++;

        TextList  & txtlist = p->getTextList();
        HexString & curline = txtlist.back();
        if ( ! _prompt.empty() ) {
            curline.str().assign(_prompt);
        } else {
            curline.str() = "";
        }

        if ( _hindx < _history.size() ) {
            curline.append(_history.at(_hindx));
            _line.assign(_history.at(_hindx));
        } else {
            _line.clear();
            _hindx = _history.size();
        }
    }
    else if ( ch == KEY_RIGHT )
    {
        ;
    }
    else if ( ch == KEY_LEFT )
    {
        ;
    }
    else if ( ch == KEY_ENTER || ch == 13 )
    {
        _isReady = true;
    }
    else if ( ch >= 32 && ch < 127 )
    {
        if ( _line.length() >= (size_t) _maxlen )
            return ch;
        if ( _echo ) {
            TextList & txtlist = p->getTextList();
            if ( txtlist.size() == 0 ) {
                std::string  f = "";
                f.append(1, ch);
                p->addText(f);
            } else {
                HexString & curline = txtlist.back();
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
LineInputHandler::setPrefix ( const std::string & prefix )
{
    _prompt = prefix;
}

/** By default, input is parsed for special manipulation such as
  * backspace or arrow keys, etc. The default behavior can be
  * bypassed by setting the parse flag to false which returns
  * all characters directly without parsing (or echo'ing).
  * This can be useful for capturing special ctrl sequences.
 **/
void
LineInputHandler::setParse ( bool parse )
{
    _parse = parse;
}

void
LineInputHandler::keepHistory ( int lines )
{
    _lines = lines;
}


}  // namespace

// _HEXES_LINEINPUTHANDLER_CPP_

