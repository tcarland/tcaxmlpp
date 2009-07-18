
#include "TnmsWxClientMain.h"
#include "ClientFrame.h"
#include "TnmsClientIOThread.h"

#include "LogFacility.h"
using namespace tcanetpp;


IMPLEMENT_APP(TnmsWxClientMain)

BEGIN_EVENT_TABLE(TnmsWxClientMain, wxApp)
    EVT_TIMER(TGUITIMER_ID, TnmsWxClientMain::OnTimer)
END_EVENT_TABLE()


TnmsWxClientMain::TnmsWxClientMain ( int msecs )
    : _guiTimer(this, TGUITIMER_ID)
{
    _stree.iomgr = new TnmsClientIOThread(&_stree);
    _stree.tree  = new TnmsTree();
    _stree.mutex = new ThreadLock();

    _guiTimer.Start(msecs);
}

TnmsWxClientMain::~TnmsWxClientMain()
{
    delete _stree.tree;
    delete _stree.mutex;
    delete _stree.iomgr;
}


bool 
TnmsWxClientMain::OnInit()
{
    LogFacility::InitThreaded();
    LogFacility::OpenLogStream("stdout", "TnmsWxClient", &std::cout);
    LogFacility::SetBroadcast(true);
    LogFacility::LogMessage("TnmsWxClient starting...");

    _stree.iomgr->start();

    ClientFrame * cf = new ClientFrame(wxT("TnmsWxClientMain"), _stree);
    cf->Show(true);

    return true;
}


int
TnmsWxClientMain::OnRun()
{
    LogFacility::LogMessage("OnRun()");
    return wxApp::OnRun();
}


int
TnmsWxClientMain::OnExit()
{
    _stree.iomgr->setAlarm();
    _stree.mutex->wait();

    LogFacility::LogMessage("OnExit()");

    return wxApp::OnExit();
}


int
TnmsWxClientMain::MainLoop()
{
    LogFacility::LogMessage("MainLoop()");
    return wxApp::MainLoop();
}


void
TnmsWxClientMain::OnTimer ( wxTimerEvent & event )
{
    return;
}



