
#include "TestHexApp.h"
#include "HexPanel.h"


namespace hexes {



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
    TestInputHandler() {}
    virtual ~TestInputHandler() {}

    int handleInput ( HexPanel * p, int ch )
    {
        return ch;
    }
};

class TestOutputHandler : public HexOutputInterface {
  public:
    TestOutputHandler() {}
    virtual ~TestOutputHandler() {}

    int handleOutput ( HexPanel * panel )
    {
        TextList::iterator  tIter;
        TextList & tlist = panel->getTextList();

        for ( tIter = tlist.begin(); tIter != tlist.end(); ++tIter )
        {
            panel->print(*tIter);
            panel->wrap();
        }
        return 1;
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

    statPanel->setOutputHandler(new TestOutputHandler());

    // we still cannot use draw() w/o display handlers, so call refresh directly
    this->setTopPanel(mainPanel);
    mainPanel->show();
    this->draw();
    ch = this->poll();

    mainPanel->print("test 1 2 3 : ");
    mainPanel->print(ch);

    statPanel->addText("the status is good");
    statPanel->addText("foobar");
    
    mainPanel->refresh();
    statPanel->redraw();

    ch = this->poll();
    std::string txt = " banana nana bo bana  test 4 5 6  this is a moderately long string to test the wrap functionality of the print() function.";
    mainPanel->print(txt);
    mainPanel->print(ch);

    mainPanel->refresh();
    ch = this->poll();

    statPanel->addText("the status is gooder");
    statPanel->addText("foobar");
    statPanel->addText("more status");
    statPanel->addText("the status is gooder2");
    statPanel->addText("foobar2");
    statPanel->addText("more status2");
    statPanel->addText("the status is gooder3");
    statPanel->addText("foobar3");
    statPanel->addText("more status3");
    statPanel->addText("the status is gooder4");
    statPanel->addText("foobar4");
    statPanel->addText("more status4");

    this->draw();
    ch = this->poll();

    return;
}
#endif


} // namespace


