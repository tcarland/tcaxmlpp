#define _HEXES_LINEOUTPUTHANDLER_CPP_

#include "LineOutputHandler.h"
#include "HexPanel.h"


namespace hexes {


int
LineOutputHandler::handleOutput ( HexPanel * panel )
{
    TextList::iterator tIter;
    TextList & tlist = panel->getTextList();

    for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter )
    {
        panel->print(*tIter);
        panel->wrap();
    }

    return 1;
}

} // namespace

