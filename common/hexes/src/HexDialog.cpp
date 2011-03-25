#define _HEXES_HEXDIALOG_CPP_

#include "HexDialog.h"


namespace hexes {


HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog )
    : HexPanel(title, 0, 0, 0, 0),
      _dynsz(true)
{
    if ( ! dialog.empty() )
        this->addText(dialog);
}

HexDialog::HexDialog ( const std::string & title,
                       const HexString   & dialog,
                       int   height, int   width,
                       int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _dynsz(false)
{
    if ( ! dialog.empty() )
        this->addText(dialog);
}

HexDialog::~HexDialog() {}


int
HexDialog::showDialog()
{
    this->initDialog();
    this->setFocus();
    this->redraw();

    int ch = this->poll();

    this->hide();

    return ch;
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

    y = ((LINES - h) / 2);
    x = ((COLS  - w) / 2);

    this->resize(h, w);
    this->moveWindow(y, x);

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

