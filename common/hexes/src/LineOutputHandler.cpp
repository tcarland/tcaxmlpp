#define _HEXES_LINEOUTPUTHANDLER_CPP_

#include "LineOutputHandler.h"
#include "HexPanel.h"


namespace hexes {


int
LineOutputHandler::handleOutput ( HexPanel * panel )
{
    TextList::iterator tIter;
    TextList & tlist = panel->getTextList();

    int ht = panel->height();
    int wd = panel->width();
    int ln = 1;
    int bw = 2;  //border width

    if ( panel->drawBorder() ) {
        ht -= bw;
        wd -= bw;
        panel->move(1, 1);
    }

    for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter, ++ln )
    {
        if ( ln > ht && panel->scrollable() )
            panel->scrollLine();

        panel->print(*tIter);

        if ( ln < ht && (size_t) ln != tlist.size() )
            panel->wrap();
        else
            panel->move(panel->curY(), 1);

    }

    return 1;
}


} // namespace

