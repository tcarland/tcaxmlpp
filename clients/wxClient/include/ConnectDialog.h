#ifndef _CONNECTDIALOG_H_
#define _CONNECTDIALOG_H_

#include <wx/wx.h>

namespace tnmsclient {


class ConnectDialog : public wxDialog {

  public:

    ConnectDialog ( const wxString & title );

    virtual ~ConnectDialog();

  public: 

    wxTextCtrl *   tcServer;
    wxTextCtrl *   tcPort;
    wxTextCtrl *   tcUser;
    wxTextCtrl *   tcPass;
};

} // namespace

#endif // _CONNECTDIALOG_H_

