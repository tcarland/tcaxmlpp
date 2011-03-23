#define _HEXES_HEXDIALOG_CPP_

#include "HexDialog.h"


namespace hexes {


HexDialog::HexDialog ( const std::string & title,
                       const std::string & txt,
                       int   height, int   width,
                       int   starty, int   startx )
    : HexPanel(title, height, width, starty, startx),
      _dialog(txt)
{
    this->initDialog();
}

HexDialog::~HexDialog() {}


int
HexDialog::showDialog()
{
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
    int rows = LINES - 4;
    int cols = COLS - 4;
    int h, w, y, x;
            
    w = _dialog.length() + 2;
    h = 4;

    if ( this->width() == 0 ) {
        if ( _dialog.length() > (size_t) cols )
            w = COLS * .6;
        int tmpw = w - 2;
        while ( (size_t) tmpw > _dialog.length() ) {
            tmpw -= (w - 2);
            h++;
        }
    }

    this->addText(_dialog);

    this->resize(h, w);

    y = cols - (w + 1);
    x = rows - (h + 1);

    this->moveWindow(y, x);
}

} // namespace


// _HEXES_HEXDIALOG_CPP_

