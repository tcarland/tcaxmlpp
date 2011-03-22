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

    this->setCursor(1);

    LineInputHandler * cinput;

    mainPanel = this->createPanel("main", LINES-statheight-1-conheight, COLS, 1, 0);
    statPanel = this->createPanel("status", statheight, COLS, LINES-statheight-conheight, 0);
    conPanel  = this->createPanel("console", conheight, COLS, LINES-conheight, 0);

    mainPanel->setBorderColor(HEX_MAGENTA);
    mainPanel->setBorderActiveColor(HEX_GREEN);
    mainPanel->setWindowTitle(" Main ", HEX_GREEN);

    statPanel->setBorderColor(HEX_MAGENTA);
    statPanel->setBorderActiveColor(HEX_GREEN);
    statPanel->setTextColor(HEX_GREEN);
    statPanel->setWindowTitle(" Status ", HEX_GREEN);

    conPanel->setDrawBorder(false);
    conPanel->setDrawTitle(false);

    std::string top = "  TestHexApp Version 0.1a  -  libhexes ";
    top.append(LIBHEXES_VERSION);
    this->print(0, 1, top, HEX_RED, HEX_BOLD);

    mainPanel->setOutputHandler(new LineOutputHandler());
    statPanel->setOutputHandler(new LineOutputHandler());
    conPanel->setInputHandler(new LineInputHandler());
    mainPanel->enableScroll(true);
    statPanel->enableScroll(true);

    this->setFocus(conPanel);
    
    cinput = (LineInputHandler*) conPanel->getInputHandler();

    std::string  cmd, prompt;

    prompt = " > ";

    conPanel->addText(prompt);
    cinput->setPrefix(prompt);

    while ( ! alarm ) 
    {
        bool wcmd = false;
        this->draw();

        cmd = "";
        ch  = this->poll();

        while ( ! cinput->isReady() ) {
            std::ostringstream  ostr;
            ostr << "ch = " << ch << "   '" << (char) ch << "'";

            if ( wcmd ) {
                wcmd = false;
                cinput->setParse(true);
                ostr << " <window command>";
                if ( ch == 'n' )
                    this->setFocusNext();
                else if ( ch == 'p' )
                    this->setFocusPrev();
            }

            if ( ch == 23 ) {
                wcmd = true;
                cinput->setParse(false);
            }

            //if ( ch >= 32 && ch < 128 )
                mainPanel->addText(ostr.str());

            this->draw();

            ch = this->poll();
        }

        cmd = cinput->getLine();
        conPanel->setText(prompt);

        if ( cmd.size() > 0 )
            statPanel->addText(cmd);

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


