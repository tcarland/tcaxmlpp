#ifndef _CLIENTFRAME_H_
#define _CLIENTFRAME_H_

#include <map>
#include <string>

#include <wx/wx.h>

#include "TnmsWxTree.h"

#include "TnmsClient.h"
using namespace tnmsCore;

#define TNMS_ID_CONNECT  7010
#define TNMS_ID_DISCONN  7011


class ClientFrame : public wxFrame {

  public:

    ClientFrame ( const wxString & title, 
                  TnmsTree_R     & tree );


    void        OnSelect        ( wxCommandEvent & event );
    void        OnConnect       ( wxCommandEvent & event );
    void        OnDisconnect    ( wxCommandEvent & event );
    void        OnQuit          ( wxCommandEvent & event );

    void        OnTimer         ( wxTimerEvent & event );

  protected:

    void        initMenuBar();


  protected:


    struct  ClientInstance
    {
        std::string  username;
        std::string  password;

        std::string  servername;
        uint16_t     port;

        TnmsClient*  client;

        ClientInstance() : port(0), client(NULL) {}
    };
    
    typedef std::map<wxString, ClientInstance>  ClientMap;

  private:

    TnmsTree_R &        _stree;
    
    TnmsWxTree*         _tree;

    ClientMap           _clientMap;

    wxPanel*            _mainPanel;
    wxMenuBar*          _menuBar;
    wxMenu*             _menuFile;

    wxGenericDirCtrl*   _gdir;
    wxListCtrl*         _lCtrl1;
    wxListCtrl*         _lCtrl2;

};


#endif //_CLIENTFRAME_H_

