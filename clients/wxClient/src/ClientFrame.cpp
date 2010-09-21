#define _CLIENTFRAME_CPP_

#include "ClientFrame.h"
#include "TnmsClientIOThread.h"

#include <wx/treectrl.h>
#include <wx/dirctrl.h>
#include <wx/dir.h>
#include <wx/splitter.h>
#include <wx/aboutdlg.h>

#include "ConnectDialog.h"

#include "StringUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;



// ----------------------------------------------------------------------

wxString
ClientFrame::_Version = _T("0.3.40");

// ----------------------------------------------------------------------

ClientFrame::ClientFrame ( const wxString & title, TnmsTree_R * tree )
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(950,600)),
      _stree(tree),
      _tree(NULL)
{
    wxSplitterWindow * spl1 = new wxSplitterWindow(this, -1); 
    wxSplitterWindow * spl2 = new wxSplitterWindow(spl1, -1);

    _tree   = new TnmsWxTree(spl1, TNMS_ID_TREE, wxT("tnms"), 
                wxPoint(-1, -1), spl1->GetSize());
    _tree->SetTnmsTree(_stree);

    spl1->SplitVertically(_tree, spl2);
    
    _mlist  = new MetricListView(spl2, TNMS_ID_MLIST,  
                 wxPoint(-1, -1), wxSize(-1, -1), wxSUNKEN_BORDER);
    _lCtrl2 = new wxListCtrl(spl2, -1, 
                 wxPoint(-1, -1), wxSize(-1, -1), wxLC_LIST);
    
    spl2->SplitHorizontally(_mlist, _lCtrl2);

    //-------------------------------------------------------
    //  Tree events

    //Connect(TNMS_ID_WXTREE, wxEVT_COMMAND_TREE_ITEM_EXPANDED, 
        //wxCommandEventHandler(ClientFrame::OnExpandItem));
    //Connect(TNMS_ID_WXTREE, wxEVT_COMMAND_TREE_ITEM_EXPANDED, 
        //wxTreeEventHandler(TnmsWxTree::OnExpandItem));

    //Connect(TNMS_ID_WXTREE, wxEVT_COMMAND_TREE_ITEM_COLLAPSING,
        //wxTreeEventHandler(TnmsWxTree::OnCollapseItem));

    Connect(TNMS_ID_WXTREE, wxEVT_COMMAND_TREE_SEL_CHANGED,
        wxTreeEventHandler(ClientFrame::OnTreeSelect));
    //Connect(TNMS_ID_WXTREE, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
        //wxTreeEventHandler(TnmsWxTree::OnSelect));

    Connect(TNMS_ID_WXTREE, wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
        wxTreeEventHandler(ClientFrame::OnTreeContext));

    Connect(TNMS_ID_WXTREE, wxEVT_COMMAND_TREE_DELETE_ITEM,
        wxTreeEventHandler(TnmsWxTree::OnDelete));
    
    //-------------------------------------------------------
    //  Metric List events
    Connect(TNMS_ID_MLIST, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
        wxListEventHandler(ClientFrame::OnListActivate));
    Connect(TNMS_ID_MLIST, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,
        wxListEventHandler(ClientFrame::OnListContext));

    this->initMenuBar();

    CreateStatusBar(3);
    Center();
}


ClientFrame::~ClientFrame() {}


void
ClientFrame::initMenuBar()
{
    _menuBar   = new wxMenuBar();
    _menuFile  = new wxMenu();
    _menuAbout = new wxMenu();

    // Top-level :  File
    _menuFile->Append(new wxMenuItem(_menuFile, TNMS_ID_CONNECT, wxT("&Connect")));
    _menuFile->Append(new wxMenuItem(_menuFile, TNMS_ID_DISCONN, wxT("&Disconnect")));
    _menuFile->AppendSeparator();
    _menuFile->Append(new wxMenuItem(_menuFile, wxID_EXIT, wxT("&Quit")));
    _menuAbout->Append(new wxMenuItem(_menuAbout, TNMS_ID_VERSION, wxT("&Version")));
    _menuBar->Append(_menuFile, wxT("&File"));
    _menuBar->Append(_menuAbout, wxT("&About"));

    SetMenuBar(_menuBar);

    Connect(TNMS_ID_CONNECT, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(ClientFrame::OnConnect));
    Connect(TNMS_ID_DISCONN, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(ClientFrame::OnDisconnect));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(ClientFrame::OnQuit));
    Connect(TNMS_ID_VERSION, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(ClientFrame::OnVersion));
}


void
ClientFrame::OnListActivate ( wxListEvent & event )
{
    LogFacility::LogMessage("ClientFrame::OnActivate(list)");
}


void
ClientFrame::OnTreeSelect ( wxTreeEvent & event )
{
    //wxTreeItemId  id  = event.GetItem();
    //TnmsMetric metric = _tree->GetItemMetric(id);
    //LogFacility::LogMessage("ClientFrame::OnSelect(tree)");
}


void
ClientFrame::OnTreeContext ( wxTreeEvent & event )
{
    LogFacility::LogMessage("ClientFrame::OnTreeContext() ");
    
    wxTreeItemId  id  = event.GetItem();
    TnmsMetric metric = _tree->GetItemMetric(id);
    std::string name  = metric.getElementName();

    wxMenu    * menu  = new wxMenu();
    wxPoint     point = wxGetMousePosition();
    
    point = ScreenToClient(point);

    menu->AppendRadioItem(wxID_ANY, wxT("Nothing"), wxT(""));
    menu->AppendRadioItem(TNMS_ID_SUBSCRIBE_ITEM,   wxT("Subscribe to Item"), wxT(""));
    menu->AppendRadioItem(TNMS_ID_UNSUBSCRIBE_ITEM, wxT("Unsubscribe to Item"), wxT(""));
    menu->AppendRadioItem(TNMS_ID_SUBSCRIBE_LEVEL, wxT("Subscribe to Level"), wxT(""));
    menu->AppendRadioItem(TNMS_ID_UNSUBSCRIBE_LEVEL, wxT("Unsubscribe to Level"), wxT(""));
    
    PopupMenu(menu, point);

    if ( menu->IsChecked(TNMS_ID_SUBSCRIBE_ITEM) )
    {
        LogFacility::LogMessage(" OnTreeContext Subscribe Item " + name);
        this->Subscribe(name, _mlist->Subscriber());
    }
    else if ( menu->IsChecked(TNMS_ID_UNSUBSCRIBE_ITEM) )
    {
        LogFacility::LogMessage(" OnTreeContext Unsubscribe Item " + name);
        this->Unsubscribe(name, _mlist->Subscriber());
        _mlist->RemoveMetric(name);
    }
    else if ( menu->IsChecked(TNMS_ID_SUBSCRIBE_LEVEL) )
    {
        name.append("/");
        LogFacility::LogMessage(" OnTreeContext Subscribe Level " + name);
        this->Subscribe(name, _mlist->Subscriber());
    }
    else if ( menu->IsChecked(TNMS_ID_UNSUBSCRIBE_LEVEL) )
    {
        _mlist->Unsubscribe(name);
        name.append("/");
        LogFacility::LogMessage(" OnTreeContext Unsubscribe Level " + name);
        this->Unsubscribe(name, _mlist->Subscriber());
    }

    return;
}


void
ClientFrame::OnListContext ( wxListEvent & event )
{
    LogFacility::LogMessage("ClientFrame::OnListContext() ");
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

    Connection  cl;
    cl.servername  = StringUtils::wtocstr(servname.c_str());
    cl.username    = StringUtils::wtocstr(username.c_str());
    cl.password    = StringUtils::wtocstr(password.c_str());
    cl.port        = StringUtils::fromString<uint16_t>(StringUtils::wtocstr(portname.c_str()));
    cl.client      = new TnmsClient(_stree->tree);
    cl.enabled     = true;

    this->SetStatusText(wxT("Connecting to ") + username + wxT("@") + servname, 1);

    cl.client->openConnection(cl.servername, cl.port);
    cl.client->login(cl.username, cl.password);
    _stree->iomgr->addClient(cl.client);
    
    username.append(_T("@")).append(servname).append(_T(":")).append(portname);
    _clientMap[username] = cl;

    dlg->Destroy();
}

void
ClientFrame::OnDisconnect ( wxCommandEvent & event )
{
    this->DropAllConnections();
}


void
ClientFrame::OnQuit ( wxCommandEvent & event )
{
    Close(true);
}

void
ClientFrame::OnVersion ( wxCommandEvent & event )
{
    wxAboutDialogInfo  info;
    info.SetName(_T("TnmsWxClient"));
    info.SetVersion(ClientFrame::_Version);
    info.SetDescription(_T("Test client interface to the Tnms system"));
    info.SetCopyright(_T("(c) 2010 Timothy Charlton Arland"));
    ::wxAboutBox(info);
}

void
ClientFrame::OnExpandItem ( wxCommandEvent & event )
{
    //LogFacility::LogMessage("ClientFrame::OnExpandItem");
}


void
ClientFrame::OnCollapseItem ( wxCommandEvent & event )
{
    //LogFacility::LogMessage("ClientFrame::OnCollapseItem");
}


void
ClientFrame::OnDelete ( wxCommandEvent & event )
{
    LogFacility::LogMessage("ClientFrame::OnDelete ");
}


void
ClientFrame::OnTimer ( wxTimerEvent & event )
{    
    ClientMap::iterator  cIter;

    for ( cIter = _clientMap.begin(); cIter != _clientMap.end(); ++cIter )
    {
        Connection & cw = cIter->second;

        if ( ! cw.enabled )
            continue;

        if ( cw.client->isAuthorized() && ! cw.req ) 
        {
            _stree->tree->request("*", _stree->notifier);
            cw.req = true;
            this->SetStatusText(wxT("Connection active"), 1);
        }
        else if ( ! cw.client->isConnected() )
        {
            this->SetStatusText(wxT("No connection"), 1);
        }
    }

    _tree->Sync();
    _mlist->Sync();

    return;
}

void
ClientFrame::DropAllConnections()
{
    // for now just drop all our connections
    ClientMap::iterator  cIter;
    for ( cIter = _clientMap.begin(); cIter != _clientMap.end(); ++cIter )
    {
        Connection & cw = cIter->second;

        if ( cw.client ) {
            cw.enabled = false;
            if ( !  _stree->iomgr->removeClient(cw.client) )
                this->SetStatusText(wxT("Error Removing Client"), 1);
            cw.client->close();
        }
    }
    _clientMap.clear();

    return;
}


bool
ClientFrame::Subscribe ( const std::string & name, TreeSubscriber * sub )
{
    if ( _tree->Subscribe(name, sub) == 1 )
        this->sendClientSub(name);

    return true;
}

bool
ClientFrame::Unsubscribe ( const std::string & name, TreeSubscriber * sub )
{
    if ( _tree->Unsubscribe(name, sub) == 0 )
        this->sendClientUnsub(name);

    return true;
}

void
ClientFrame::sendClientSub ( const std::string & name )
{
    _stree->mutex->lock();
    ClientMap::iterator  cIter;
    for ( cIter = _clientMap.begin(); cIter != _clientMap.end(); ++cIter )
        cIter->second.client->subscribe(name);
    _stree->mutex->unlock();
}


void
ClientFrame::sendClientUnsub ( const std::string & name )
{
    _stree->mutex->lock();
    ClientMap::iterator  cIter;
    for ( cIter = _clientMap.begin(); cIter != _clientMap.end(); ++cIter )
        cIter->second.client->unsubscribe(name);
    _stree->mutex->unlock();
}


