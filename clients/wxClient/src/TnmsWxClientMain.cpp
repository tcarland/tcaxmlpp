#define _TNMSWXCLIENTMAIN_CPP_

#include "TnmsWxClientMain.h"

#include "ClientFrame.h"
#include "TnmsClientIOThread.h"
#include "ClientSubscriber.h"

#include "LogFacility.h"
using namespace tcanetpp;



// ----------------------------------------------------------------------

IMPLEMENT_APP(TnmsWxClientMain)

BEGIN_EVENT_TABLE(TnmsWxClientMain, wxApp)
    EVT_TIMER(TGUITIMER_ID, TnmsWxClientMain::OnTimer)
END_EVENT_TABLE()


// ----------------------------------------------------------------------

TnmsWxClientMain::TnmsWxClientMain ( int msecs )
    : _guiTimer(this, TGUITIMER_ID),
      _mframe(NULL)
{
    _stree.tree     = new TnmsTree();
    _stree.notifier = new ClientSubscriber();
    _stree.mutex    = new ThreadLock();
    _stree.iomgr    = new TnmsClientIOThread(_stree.tree, _stree.mutex);

    _guiTimer.Start(msecs);
}

TnmsWxClientMain::~TnmsWxClientMain()
{
    delete _stree.iomgr;
    delete _stree.mutex;
    delete _stree.notifier;
    delete _stree.tree;
}


bool 
TnmsWxClientMain::OnInit()
{
    LogFacility::InitThreaded();
    LogFacility::OpenLogStream("stdout", "TnmsWxClient", &std::cout);
    LogFacility::SetBroadcast(true);
    LogFacility::SetDebug(true);
    LogFacility::LogMessage("TnmsWxClient starting...");

    _stree.iomgr->start();

    _mframe = new ClientFrame(wxT("TnmsWxClientMain"), &_stree);
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

    _stree.iomgr->setAlarm();
    _stree.mutex->wait();
    _stree.iomgr->stop();
    
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



