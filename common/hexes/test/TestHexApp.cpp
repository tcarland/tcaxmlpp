#define _HEXES_TESTHEXAPP_CPP_

#include <unistd.h>
#include <sstream>

#include "TestHexApp.h"

#include "HexPanel.h"
#include "LineInputHandler.h"
#include "LineOutputHandler.h"

namespace hexes {

TestHexApp::TestHexApp()
    : mainPanel(NULL),
      statPanel(NULL),
      conPanel(NULL),
      _conheight(3),
      _titleheight(1),
      _statheight(0),
      _mainheight(0)
{}

void
TestHexApp::resize()
{
    int  statstarty, constarty;
    int  ht;

    ht = this->height();

    _statheight = (ht * .33) - _conheight;
    _mainheight = ht - _statheight - _conheight - _titleheight;
    statstarty  = ht - _statheight - _conheight;
    constarty   = ht - _conheight;

    mainPanel->resize(_mainheight, this->width());
    statPanel->resize(_statheight, this->width());
    conPanel->resize(_conheight, this->width());
    statPanel->erase();
    statPanel->moveWindow(statstarty, 0);
    conPanel->erase();
    conPanel->moveWindow(constarty, 0);
}

void
TestHexApp::run()
{
    bool alarm      = false;
    int  conheight  = 3;
    int  statheight = (LINES * .33) - conheight;
    int  ch;

    //init_pair(1, COLOR_WHITE, COLOR_BLACK);

    this->setCursor(1);

    LineInputHandler * cinput;

    mainPanel = this->createPanel("main", LINES-statheight-1-conheight, COLS, 1, 0);
    statPanel = this->createPanel("status", statheight, COLS, LINES-statheight-conheight, 0);
    conPanel  = this->createPanel("console", conheight, COLS, LINES-conheight, 0);

    statPanel->setTextColor(HEX_GREEN_BLACK);

    conPanel->drawBorder(false);
    conPanel->drawTitle(false);

    std::string top = "  TestHexApp Version 0.1a  -  libhexes ";
    top.append(LIBHEXES_VERSION);
    this->print(0, 1, top, HEX_RED_BLACK, HEX_BOLD);

    mainPanel->setOutputHandler(new LineOutputHandler());
    statPanel->setOutputHandler(new LineOutputHandler());
    conPanel->setInputHandler(new LineInputHandler());
    mainPanel->enableScroll(true);
    statPanel->enableScroll(true);

    this->setTopPanel(conPanel);
    
    cinput = (LineInputHandler*) conPanel->getInputHandler();

    std::string cmd;

    conPanel->addText(" > ");

    while ( ! alarm ) 
    {
        this->draw();

        cmd = "";
        ch = this->poll();

        while ( ! cinput->isReady() ) {
            std::ostringstream  ostr;
            ostr << "ch = " << ch << "   '" << (char) ch << "'";

            if ( ch >= 32 && ch < 128 )
                mainPanel->addText(ostr.str());

            this->draw();

            ch = this->poll();
        }

        cmd = cinput->getLine();
        conPanel->setText(" > ");

        if ( cmd.size() > 0 )
            statPanel->addText(cmd);

        //statPanel->redraw();

        if ( cmd.compare("/quit") == 0 )
            alarm = true;
        else if ( cmd.compare("/clear") == 0 )
            mainPanel->clear();
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


