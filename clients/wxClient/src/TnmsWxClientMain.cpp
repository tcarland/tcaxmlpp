
#include "TnmsWxClientMain.h"
#include "ClientFrame.h"

#include "LogFacility.h"
using namespace tcanetpp;


IMPLEMENT_APP(TnmsWxClientMain)


bool TnmsWxClientMain::OnInit()
{
    LogFacility::OpenLogStream("TnmsWxClient", "stdout", &std::cout);
    LogFacility::SetBroadcast(true);

    ClientFrame * cf = new ClientFrame(wxT("TnmsWxClientMain"));
    cf->Show(true);

    return true;
}



