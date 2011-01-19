
#include <sstream>


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
    int conheight  = 3;
    int statheight = (LINES * .33) - conheight;
    int ch;

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    HexPanel *mainPanel, *statPanel, *conPanel; 

    mainPanel = this->createPanel("main", LINES-statheight-1-conheight, COLS, 1, 0);
    statPanel = this->createPanel("status", statheight, COLS, LINES-statheight-conheight, 0);
    conPanel  = this->createPanel("console", conheight, COLS, LINES-conheight, 0);
    conPanel->drawBorder(false);
    conPanel->drawTitle(false);

    std::string top = "  TestHexApp Version 0.1 ";
    this->print(0, 1, top);

    //statPanel->setOutputHandler(new TestOutputHandler());
    
    statPanel->enableScroll(true);
    this->setTopPanel(conPanel);
    
    this->draw();

    conPanel->print(" > ", false);
    ch = this->poll();
    std::string cmd = "";
    cmd.append(1, ch);

    std::ostringstream ostr;

    while ( ch != 13 || ch !=15 ) {
        conPanel->print(ch);
        cmd.append(1, ch);
        ostr << " ch: " << ch;
        mainPanel->print(ostr.str());
        conPanel->refresh();
        mainPanel->refresh();

        ch = this->poll();
    }

    mainPanel->print("test 1 2 3 : ");
    mainPanel->print(cmd);

    statPanel->addText("the status is good");
    statPanel->addText("foobar");
    
    mainPanel->refresh();
    statPanel->redraw();

    ch = this->poll();
    std::string txt = " banana nana bo bana  ...  test 4 5 6 ...  this is a moderately long string to test the wrap functionality of the print() function.";
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


