#ifndef _CLIENTFRAME_H_
#define _CLIENTFRAME_H_

#include <wx/wx.h>

#include "TnmsWxTree.h"


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


  protected:

    void        initMenuBar();


  private:

    TnmsTree_R &        _stree;

    wxPanel*            _mainPanel;
    wxMenuBar*          _menuBar;
    wxMenu*             _menuFile;

    TnmsWxTree*         _tree;

    wxGenericDirCtrl*   _gdir;
    wxListCtrl*         _lCtrl1;
    wxListCtrl*         _lCtrl2;

};


#endif //_CLIENTFRAME_H_

