#define _TNMSWXCLIENTMAIN_CPP_

#include "TnmsWxClientMain.h"

#include "ClientFrame.h"
#include "ClientIOThread.h"
#include "ClientSubscriber.h"
#include "ClientTreeMutex.h"

#include "LogFacility.h"
using namespace tcanetpp;



// ----------------------------------------------------------------------

IMPLEMENT_APP(tnmsClient::TnmsWxClientMain)
BEGIN_EVENT_TABLE(tnmsClient::TnmsWxClientMain, wxApp)
    EVT_TIMER(TGUITIMER_ID, tnmsClient::TnmsWxClientMain::OnTimer)
END_EVENT_TABLE()

namespace tnmsClient {


std::string
TnmsWxClientMain::_Version = "0.4.1";

// ----------------------------------------------------------------------

TnmsWxClientMain::TnmsWxClientMain ( int msecs )
    : _mtree(new ClientTreeMutex()),
      _iomgr(new ClientIOThread(_mtree)),
      _mframe(NULL),
      _guiTimer(this, TGUITIMER_ID)
{
    _guiTimer.Start(msecs);
}


TnmsWxClientMain::~TnmsWxClientMain()
{
    delete _mtree;
    delete _iomgr;
}


bool 
TnmsWxClientMain::OnInit()
{
    LogFacility::InitThreaded();
    LogFacility::OpenLogStream("stdout", "TnmsWxClient", &std::cout);
    LogFacility::SetBroadcast(true);
    LogFacility::SetDebug(true);
    LogFacility::LogMessage("TnmsWxClient starting...");

    _iomgr->start();

    _mframe = new ClientFrame(wxT("TnmsWxClient"), _mtree, _iomgr);
    _mframe->Show(true);

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

    _mframe->DropAllConnections();

    _iomgr->setAlarm();
    _mtree->wait();
    _iomgr->stop();
    
    LogFacility::RemoveLogStream("stdout");
    LogFacility::CloseLogFacility();

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
    _mframe->OnTimer(event);
    return;
}


} // namespace

