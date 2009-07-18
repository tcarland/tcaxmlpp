#define _CLIENTFRAME_CPP_

#include "ClientFrame.h"
#include "TnmsClientIOThread.h"

#include <wx/treectrl.h>
#include <wx/dirctrl.h>
#include <wx/dir.h>
#include <wx/splitter.h>

#include "ConnectDialog.h"

#include "StringUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;



ClientFrame::ClientFrame ( const wxString & title, TnmsTree_R & tree )
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(500,300)),
      _stree(tree)
{
    _menuBar   = new wxMenuBar();
    _menuFile  = new wxMenu();

    wxSplitterWindow * spl1 = new wxSplitterWindow(this, -1); 
    wxSplitterWindow * spl2 = new wxSplitterWindow(spl1, -1);

    _tree   = new TnmsWxTree(spl1, wxID_TREECTRL, wxT("tcanms"), 
                wxPoint(-1, -1), spl1->GetSize());
    _tree->SetTnmsTree(&_stree);

    _gdir   = new wxGenericDirCtrl(spl2, -1, wxT("/home/"),
                wxPoint(-1, -1), wxSize(-1, -1), wxDIRCTRL_DIR_ONLY);
    
    _lCtrl1 = new wxListCtrl(spl2, -1, wxPoint(-1, -1), wxSize(-1, -1), wxLC_LIST);
    _lCtrl2 = new wxListCtrl(spl2, -1, wxPoint(-1, -1), wxSize(-1, -1), wxLC_LIST);
    
    spl2->SplitHorizontally(_lCtrl1, _lCtrl2);
    spl1->SplitVertically(_tree, spl2);

    Connect(_gdir->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, 
        wxCommandEventHandler(ClientFrame::OnSelect));

    this->initMenuBar();

    Center();
}



void
ClientFrame::initMenuBar()
{
    // Top-level :  File

    //_menuFile->Append(TNMS_ID_CONNECT, wxT("&Connect"));
    //_menuFile->Append(TNMS_ID_DISCONN, wxT("&Disconnect"));
    //_menuFile->AppendSeparator();

    //wxMenuItem * quitMenu = new wxMenuItem(_menuFile, wxID_EXIT, wxT("&Quit"));

    _menuFile->Append(new wxMenuItem(_menuFile, TNMS_ID_CONNECT, wxT("&Connect")));
    _menuFile->Append(new wxMenuItem(_menuFile, TNMS_ID_DISCONN, wxT("&Disconnect")));
    _menuFile->AppendSeparator();
    _menuFile->Append(new wxMenuItem(_menuFile, wxID_EXIT, wxT("&Quit")));
    _menuBar->Append(_menuFile, wxT("&File"));

    SetMenuBar(_menuBar);

    Connect(TNMS_ID_CONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ClientFrame::OnConnect));
    Connect(TNMS_ID_DISCONN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ClientFrame::OnDisconnect));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ClientFrame::OnQuit));
}


void
ClientFrame::OnConnect ( wxCommandEvent & event )
{
    ConnectDialog * dlg = new ConnectDialog(wxT("Connect"));

    if ( dlg->ShowModal() == wxID_OK ) {
        LogFacility::LogMessage("OnConnect() OK");
    } else {
        LogFacility::LogMessage("OnConnect() Cancelled");
        return;
    }

    wxString  servname = dlg->tcServer->GetValue();
    wxString  portname = dlg->tcPort->GetValue();
    wxString  username = dlg->tcUser->GetValue();
    wxString  password = dlg->tcPass->GetValue();

    ClientInstance  cl;
    cl.servername  = StringUtils::wtocstr(servname.c_str());
    cl.username    = StringUtils::wtocstr(username.c_str());
    cl.password    = StringUtils::wtocstr(password.c_str());
    cl.port        = StringUtils::fromString<uint16_t>(StringUtils::wtocstr(portname.c_str()));
    cl.client      = new TnmsClient(_stree.tree);

    //uint16_t pn = StringUtils::fromString<uint16_t>(port);

    cl.client->openConnection(cl.servername, cl.port);
    _stree.iomgr->addClient(cl.client);
    
    //user.append("@").append(sn).append(":").append(port);
    username.append(_T("@")).append(servname).append(_T(":")).append(portname);
    _clientMap[username] = cl;

    dlg->Destroy();
}

void
ClientFrame::OnDisconnect ( wxCommandEvent & event )
{
    // for now just drop all our connections
    ClientMap::iterator  cIter;
    for ( cIter = _clientMap.begin(); cIter != _clientMap.end(); ++cIter )
    {
        ClientInstance & clin = cIter->second;

        if ( clin.client ) {
            clin.client->close();
            _stree.iomgr->removeClient(clin.client);
        }
    }
    _clientMap.clear();
}


void
ClientFrame::OnSelect ( wxCommandEvent & event )
{
    /*
    wxString  filename, path;
    bool      cont;
    int       i;

    path  = _gdir->GetPath();
    i     = 0;
    wxDir dir(path); 

    cont  = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);

    _lCtrl1->ClearAll();

    while ( cont ) 
    {
        _lCtrl1->InsertItem(i, filename);
        cont = dir.GetNext(&filename);
        i++;
    }

    return;    
    */

}


void
ClientFrame::OnQuit ( wxCommandEvent & event )
{
    Close(true);
}


