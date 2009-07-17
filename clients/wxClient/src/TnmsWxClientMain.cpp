
#include "TnmsWxClientMain.h"
#include "ClientFrame.h"

#include "LogFacility.h"
using namespace tcanetpp;


IMPLEMENT_APP(TnmsWxClientMain)

BEGIN_EVENT_TABLE(TnmsWxClientMain, wxApp)
    EVT_TIMER(TGUITIMER_ID, TnmsWxClientMain::OnTimer)
END_EVENT_TABLE()


TnmsWxClientMain::TnmsWxClientMain ( int msecs )
    : _guiTimer(this, TGUITIMER_ID)
{
    _guiTimer.Start(msecs);
}

TnmsWxClientMain::~TnmsWxClientMain() {}

bool 
TnmsWxClientMain::OnInit()
{
    LogFacility::OpenLogStream("stdout", "TnmsWxClient", &std::cout);
    LogFacility::SetBroadcast(true);

    LogFacility::LogMessage("TnmsWxClient starting...");

    ClientFrame * cf = new ClientFrame(wxT("TnmsWxClientMain"));
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



