#define _HEXES_HEXDIALOG_CPP_

#include "HexDialog.h"


namespace hexes {


HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog )
    : HexPanel(title, 0, 0, 0, 0),
      _rescnt(0),
      _dynsz(true),
      _char(true),
      _echo(false)
{
    if ( ! dialog.empty() )
        this->addText(dialog);
}

HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog,
                       int   height, int   width,
                       int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _rescnt(0),
      _dynsz(false),
      _char(true),
      _echo(false)
{
    if ( ! dialog.empty() )
        this->addText(dialog);
}

HexDialog::~HexDialog() {}


int
HexDialog::showDialog()
{
    int  ch;

    this->initDialog();
    this->enableScroll(false);
    this->setFocus();
    this->redraw();

    this->setInputHandler(new LineInputHandler(_rescnt));
    LineInputHandler * cinput = (LineInputHandler*) this->getInputHandler();
    cinput->setEcho(false);

    ch = this->poll();

    while ( ! cinput->isReady() ) 
    {
        if ( _char )
            break;

        if ( _echo ) {
            TextList    & tlist = this->getTextList();
            HexString   & inl   = tlist.back();
            std::string & ln    = cinput->getLine();
            int  indx           = ln.length() - 1;

            if ( ln.length() < (size_t) _rescnt ) {
                inl.str().replace(indx, 1, 1, ch);
            } else if ( _echo ) { 
                inl.str().replace(indx, 1, 1, ch);
                _echo = false;
            }
        }

        this->redraw();
        ch = this->poll();
    }
    _result = cinput->getLine();

    this->hide();

    return ch;
}


std::string
HexDialog::getResult()
{
    return _result;
}

void
HexDialog::setCharOnly ( bool chin )
{
    _char = chin;
}

void
HexDialog::echoResults ( bool echo )
{
    _echo = true;
}

void
HexDialog::setMaxInput ( int count )
{
    if ( count > 1 )
        _char = false;
    _rescnt = count;
}

void
HexDialog::initDialog()
{
    int h, w, y, x;
            
    h = this->getLineCount();
    w = this->getLongestLine();

    if ( _echo ) {
        std::string tline;
        if ( _rescnt == 0 )
            _rescnt = w + 1;
        tline.append(_rescnt, '_');
        this->addText(tline);
        h += 1;
    }

    if ( this->getDrawBorder() ) {
        h += 2;
        w += 2;
    }

    y = ((LINES - h) / 2) - 1;
    x = ((COLS  - w) / 2);

    if ( _dynsz ) {
        this->resize(h, w);
    	this->moveWindow(y, x);
    }

    return;
}

size_t
HexDialog::getLongestLine()
{
    TextList & tlist = this->getTextList();
    TextList::iterator  tIter;
    size_t len = 0;

    for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter )
    {
        HexString & line = *tIter;
        if ( line.length() < len )
            continue;
        len = line.length() + 1;
    }

    return len;
}

} // namespace


// _HEXES_HEXDIALOG_CPP_

