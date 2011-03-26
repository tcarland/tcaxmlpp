#define _HEXES_LINEOUTPUTHANDLER_CPP_

#include "LineOutputHandler.h"
#include "HexPanel.h"


namespace hexes {


LineOutputHandler::LineOutputHandler ( bool newline )
    : _newline(newline)
{}


int
LineOutputHandler::handleOutput ( HexPanel * panel )
{
    TextList::iterator tIter;
    TextList & tlist = panel->getTextList();

    int ht = panel->height();
    int wd = panel->width();
    int st = panel->scrollTo();
    int ln = 1;
    int bw = 2;  //border width

    if ( panel->getDrawBorder() ) {
        ht -= bw;
        wd -= bw;
        panel->move(1, 1);
    }

    if ( st > 0 && tlist.size() > (size_t) ht )
        st = tlist.size() - st;
    else
        st = 0;

    for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter, ++ln )
    {
        HexString & line = *tIter;

        if ( ln > ht && panel->scrollable() )
            panel->scrollLine();

        panel->setAttribute(COLOR_PAIR(line.color));
        if ( line.attributes > HEX_NORMAL )
            panel->setAttribute(line.attributes);

        panel->print(line.str());

        panel->unsetAttribute(COLOR_PAIR(line.color));
        if ( line.attributes > HEX_NORMAL )
            panel->unsetAttribute(line.attributes);

        if ( ln < ht && (size_t) ln != tlist.size() && _newline )
            panel->wrap();
        else
            panel->move(panel->curY(), 1);
        
        if ( st > 0 && st == ln )
            break;
    }

    return 1;
}


bool
LineOutputHandler::newline() const
{
    return this->_newline;
}


void
LineOutputHandler::setNewline ( bool newline )
{
    _newline = newline;
}

} // namespace

