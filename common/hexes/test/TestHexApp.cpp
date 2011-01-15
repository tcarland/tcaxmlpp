
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


class TestInputHandler : public HexInputInterface {
  public:
    TestInputHandler();
    virtual ~TestInputHandler() {}

    int handleInput ( HexPanel * p, int ch )
    {
        return ch;
    }
};


void
TestHexApp::run()
{
    int statheight = (LINES * .33);
    int ch;

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    HexPanel *mainPanel, *statPanel; 

    statPanel = this->createPanel("status", statheight, COLS, LINES-statheight, 0);
    mainPanel = this->createPanel("main", LINES-statheight, COLS, 0, 0);

    // we still cannot use draw() w/o display handlers, so call refresh directly
    this->setTopPanel(mainPanel);
    mainPanel->show();
    this->draw();
    ch = this->poll();

    mainPanel->print("test 1 2 3 : ");
    mainPanel->print(ch);
    statPanel->print("the status is good");
    //this->draw();
    mainPanel->refresh();
    statPanel->refresh();
    ch = this->poll();
    std::string txt = " banana nana bo bana  test 4 5 6  this is a moderately long string to test the wrap functionality of the print() function.";
    mainPanel->print(txt);
    mainPanel->print(ch);

    statPanel->redraw();
    mainPanel->refresh();
    ch = this->poll();

    return;
}
#endif


} // namespace


