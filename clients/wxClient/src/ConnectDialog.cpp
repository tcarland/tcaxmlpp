#define _CONNECTDIALOG_CPP_


#include "ConnectDialog.h"

namespace tnmsClient {


ConnectDialog::ConnectDialog ( const wxString & title )
    : wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(350, 250), 
        wxRESIZE_BORDER|wxSTAY_ON_TOP|wxCAPTION)
{
    wxPanel      * panel   = new wxPanel(this, -1);
    wxBoxSizer   * vbox    = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer * fgs  = new wxFlexGridSizer(5, 2, 10, 25);

    wxStaticText * server  = new wxStaticText(panel, -1, wxT("Server"));
    wxStaticText * port    = new wxStaticText(panel, -1, wxT("Port"));
    wxStaticText * user    = new wxStaticText(panel, -1, wxT("Username"));
    wxStaticText * pass    = new wxStaticText(panel, -1, wxT("Password"));

    tcServer  = new wxTextCtrl(panel, -1, wxT("localhost"));
    wxSize sz = tcServer->GetSize();
    sz.x  += 35;
    tcServer->SetSize(sz);

    tcPort    = new wxTextCtrl(panel, -1, wxT("15605"), wxPoint(-1, -1), sz);
    tcUser    = new wxTextCtrl(panel, -1, wxT("tnms-console"), wxPoint(-1, -1), sz);
    tcPass    = new wxTextCtrl(panel, -1, wxT("tnmsconsole11b"), wxPoint(-1, -1), wxSize(-1, -1), wxTE_PASSWORD);

    fgs->Add(-1, 20);
    fgs->Add(-1, 20);
    fgs->Add(server);
    fgs->Add(tcServer, 1, wxEXPAND);
    fgs->Add(port);
    fgs->Add(tcPort, 1, wxEXPAND);
    fgs->Add(user);
    fgs->Add(tcUser, 1, wxEXPAND);
    fgs->Add(pass);
    fgs->Add(tcPass, 1, wxEXPAND);

    wxSizer * dsizer = this->CreateButtonSizer(wxOK|wxCANCEL);

    panel->SetSizer(fgs);
    vbox->Add(panel, 0, wxALIGN_CENTER, 5 );
    vbox->Add(dsizer, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);

    SetSizer(vbox);

    Center();
}


ConnectDialog::~ConnectDialog() 
{}


} // namespace


