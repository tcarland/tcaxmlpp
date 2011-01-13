
#include "TestHexApp.h"
#include "HexPanel.h"

namespace hexes {


//draw()

#ifdef TESTHEX_1

void
TestHexApp::run()
{
    int statheight = (LINES * .10);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    HexPanel * mainPanel = new HexPanel("window 1", LINES-statheight, COLS, 0, 0);
    mainPanel->redraw();
    getch();

    mainPanel->print("test 1 2 3");
    mainPanel->refresh();
    getch();

    mainPanel->print("test 4 5 6");
    mainPanel->refresh();
    getch();

    delete mainPanel;

    return;
}

#else

void
TestHexApp::run()
{
    int statheight = (LINES * .10);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    HexPanel * mainPanel = this->createPanel("main", LINES-statheight, COLS, 0, 0);
    HexPanel * statPanel = this->createPanel("status", statheight, COLS, LINES-statheight, 0);

    // we still cannot use draw() w/o display handlers, so call refresh directly
    mainPanel->setTopPanel();
    this->draw();
    getch();

    mainPanel->print("test 1 2 3");
    statPanel->print("the status is good");
    //this->draw();
    mainPanel->refresh();
    statPanel->refresh();
    getch();
    mainPanel->print("test 4 5 6");
    statPanel->redraw();
    mainPanel->refresh();
    getch();

    return;
}
#endif


} // namespace


