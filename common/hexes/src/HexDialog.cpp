#define _HEXES_HEXDIALOG_CPP_

#include "HexDialog.h"


namespace hexes {


HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog )
    : HexPanel(title, 0, 0, 0, 0),
      _dynsz(true),
      _char(false)
{
    if ( ! dialog.empty() )
        this->addText(dialog);
}

HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog,
                       int   height, int   width,
                       int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _dynsz(false),
      _char(false)
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
    this->setFocus();
    this->redraw();

    while ( (ch = this->poll()) != KEY_ENTER && ch != 13 )
    {
        _result.append(1, ch);
        if ( _char )
            break;
    }

    this->hide();

    return ch;
}


std::string
HexDialog::getResult()
{
    return _result;
}

void
HexDialog::getCharOnly()
{
    _char = true;
}


void
HexDialog::initDialog()
{
    int h, w, y, x;
            
    h = this->getLineCount();
    w = this->getLongestLine();

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

