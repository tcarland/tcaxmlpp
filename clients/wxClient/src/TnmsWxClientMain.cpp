
#include "TnmsWxClientMain.h"
#include "ClientFrame.h"

#include "LogFacility.h"
using namespace tcanetpp;


IMPLEMENT_APP(TnmsWxClientMain)


bool TnmsWxClientMain::OnInit()
{
    LogFacility::OpenLogStream("stdout", "TnmsWxClient", &std::cout);
    LogFacility::SetBroadcast(true);

    LogFacility::LogMessage("TnmsWxClient starting...");

    ClientFrame * cf = new ClientFrame(wxT("TnmsWxClientMain"));
    cf->Show(true);

    return true;
}



