#define _HEXES_TESTHEXAPP_CPP_

#include <unistd.h>
#include <sstream>

#include "TestHexApp.h"

#include "HexPanel.h"
#include "LineInputHandler.h"
#include "LineOutputHandler.h"

namespace hexes {



void
TestHexApp::run()
{
    bool alarm      = false;
    int  conheight  = 3;
    int  statheight = (LINES * .33) - conheight;
    int  ch;

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    HexPanel *mainPanel, *statPanel, *conPanel; 
    LineInputHandler * cinput;

    mainPanel = this->createPanel("main", LINES-statheight-1-conheight, COLS, 1, 0);
    statPanel = this->createPanel("status", statheight, COLS, LINES-statheight-conheight, 0);
    conPanel  = this->createPanel("console", conheight, COLS, LINES-conheight, 0);

    conPanel->drawBorder(false);
    conPanel->drawTitle(false);

    std::string top = "  TestHexApp Version 0.1a  -  libhexes ";
    top.append(LIBHEXES_VERSION);
    this->print(0, 1, top);

    mainPanel->setOutputHandler(new LineOutputHandler());
    statPanel->setOutputHandler(new LineOutputHandler());
    conPanel->setInputHandler(new LineInputHandler());
    mainPanel->enableScroll(true);
    statPanel->enableScroll(true);

    this->setTopPanel(conPanel);
    
    cinput = (LineInputHandler*) conPanel->getInputHandler();

    std::string cmd;

    while ( ! alarm ) 
    {
        this->draw();

        cmd = "";
        conPanel->print(" > ", false);
        ch = this->poll();

        while ( ! cinput->isReady() ) {
            std::ostringstream  ostr;
            ostr << "ch = " << ch << "   '" << (char) ch << "'";
            mainPanel->addText(ostr.str());
            //mainPanel->print(ostr.str(), true);
            conPanel->refresh();
            mainPanel->redraw();

            ch = this->poll();
        }
        cmd = cinput->getLine();

        statPanel->addText(cmd);
        statPanel->redraw();

        sleep(1);

        if ( cmd.compare("quit") == 0 )
            alarm = true;
    }


    mainPanel->print("test 1 2 3 : ");
    mainPanel->print(cmd);
    mainPanel->refresh();

    statPanel->addText("the status is good");
    statPanel->addText("foobar");
    statPanel->redraw();

    std::string txt = " banana nana bo bana  ...  test 4 5 6 ...  this is a moderately long string to test the wrap functionality of the print() function.";
    mainPanel->print(txt);
    mainPanel->refresh();

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
    sleep(1);

    return;
}


} // namespace


