#define _HEXES_HEXDIALOG_CPP_

#include "HexDialog.h"


namespace hexes {


HexDialog::HexDialog ( const std::string & title,
                       const std::string & dialog )
    : HexPanel(title, 0, 0, 0, 0),
      _dialog(dialog),
      _dynsz(true)
{
    //if ( ! _dialog.empty() )
        //this->addText(_dialog);
}

HexDialog::HexDialog ( const std::string & title,
                       const std::string & dialog,
                       int   height, int   width,
                       int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _dialog(dialog),
      _dynsz(false)
{
}

HexDialog::~HexDialog() {}


int
HexDialog::showDialog()
{
    //this->initDialog();
    this->setFocus();
    this->redraw();

    int ch = this->poll();

    this->hide();
    this->refresh();

    return ch;
}


void
HexDialog::initDialog()
{
    int rows = this->height();
    int cols = this->width();
    int h, w, y, x;
            
    h = this->getLineCount();
    w = this->getLongestLine();

    if ( ! _dynsz ) {
        if ( w > cols )
            h = (w / cols);
        h += rows;
        w  = cols;
    }

    if ( _dynsz && this->getDrawBorder() ) {
        h += 2;
        w += 2;
    }

    y = ((COLS - (w+1)) / 2);
    x = ((LINES - (h+1)) / 2);

    this->resize(h, w);

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
        if ( line.str.length() < len )
            continue;
        len = line.str.length() + 2;
    }

    return len;
}

} // namespace


// _HEXES_HEXDIALOG_CPP_

