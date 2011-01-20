#define _HEXES_TESTHEXAPP_CPP_

#include <unistd.h>
#include <sstream>

#include "TestHexApp.h"
#include "HexPanel.h"


namespace hexes {



class TestInputHandler : public HexInputInterface {
  public:
    TestInputHandler() : _ready(false) {}
    virtual ~TestInputHandler() {}

    int handleInput ( HexPanel * p, int ch )
    {
        if ( _ready ) {
            _line.clear();
            _ready = false;
        }

        if ( ch == KEY_BACKSPACE || ch == 127 ) {
            int x = p->curX();
            p->move(p->curY(), x-1);
            p->print(' ');
            p->move(p->curY(), x-1);
            p->refresh();
            _line.erase(_line.size() - 1);
        } else if ( ch == KEY_UP ) {
            ;
        } else if ( ch == KEY_DOWN ) {
            ;
        } else if ( ch == KEY_RIGHT ) {
            ;
        } else if ( ch == KEY_LEFT ) {
            ;
        } else if ( ch == KEY_ENTER || ch == 13 ) {
            _ready = true;
        } else {
            p->print(ch);
            _line.append(1, ch);
        }

        return ch;
    }

    std::string getLine() { return _line; }
    bool        isReady() { return _ready; }

  protected:

    bool         _ready;
    std::string  _line;
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
    bool alarm      = false;
    int  conheight  = 3;
    int  statheight = (LINES * .33) - conheight;
    int  ch;

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    HexPanel *mainPanel, *statPanel, *conPanel; 

    mainPanel = this->createPanel("main", LINES-statheight-1-conheight, COLS, 1, 0);
    statPanel = this->createPanel("status", statheight, COLS, LINES-statheight-conheight, 0);
    conPanel  = this->createPanel("console", conheight, COLS, LINES-conheight, 0);

    conPanel->drawBorder(false);
    conPanel->drawTitle(false);

    std::string top = "  TestHexApp Version 0.1a";
    this->print(0, 1, top);

    //statPanel->setOutputHandler(new TestOutputHandler());
    TestInputHandler * cinput;
    cinput = new TestInputHandler();
    conPanel->setInputHandler(cinput);
    
    statPanel->enableScroll(true);
    this->setTopPanel(conPanel);
    

    std::string cmd;

    while ( ! alarm ) 
    {
        this->draw();

        cmd = "";
        conPanel->print(" > ", false);
        ch = this->poll();

        //while ( ch != 13 ) {
        while ( ! cinput->isReady() ) {
            std::ostringstream  ostr;
            ostr << "ch = " << ch << " ";
            //conPanel->print(ch);
            //cmd.append(1, ch);
            mainPanel->print(ostr.str(), true);
            conPanel->refresh();
            mainPanel->refresh();

            ch = this->poll();
        }
        cmd = cinput->getLine();

        statPanel->addText(cmd);
        statPanel->refresh();

        sleep(1);

        if ( cmd.compare("quit") == 0 )
            alarm = true;
    }


    mainPanel->print("test 1 2 3 : ");
    mainPanel->print(cmd);

    statPanel->addText("the status is good");
    statPanel->addText("foobar");
    
    mainPanel->refresh();
    statPanel->redraw();

    //ch = this->poll();
    std::string txt = " banana nana bo bana  ...  test 4 5 6 ...  this is a moderately long string to test the wrap functionality of the print() function.";
    mainPanel->print(txt);

    mainPanel->refresh();
    //ch = this->poll();

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
    //ch = this->poll();
    sleep(1);

    return;
}


} // namespace


