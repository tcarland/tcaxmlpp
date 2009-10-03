#ifndef _CLIENTFRAME_H_
#define _CLIENTFRAME_H_

#include <map>
#include <string>

#include <wx/wx.h>

#include "TnmsWxTree.h"
#include "MetricListView.h"

#include "TnmsClient.h"
using namespace tnmsCore;



class ClientFrame : public wxFrame {

  public:

    ClientFrame ( const wxString & title, 
                  TnmsTree_R     * tree );

    virtual ~ClientFrame();

    void          OnSelect        ( wxTreeEvent    & event );
    void          OnContext       ( wxTreeEvent    & event );

    void          OnConnect       ( wxCommandEvent & event );
    void          OnDisconnect    ( wxCommandEvent & event );
    void          OnQuit          ( wxCommandEvent & event );

    void          OnDelete        ( wxCommandEvent & event );
    void          OnExpandItem    ( wxCommandEvent & event );
    void          OnCollapseItem  ( wxCommandEvent & event );

    void          OnTimer         ( wxTimerEvent & event );


  protected:

    void        initMenuBar();


  protected:

    struct Connection
    {
        std::string  username;
        std::string  password;

        std::string  servername;
        uint16_t     port;
        bool         req;
        bool         enabled;

        TnmsClient*  client;

        Connection() : port(0), 
                       req(false),
                       enabled(false),
                       client(NULL)
        {}
    };
    
    typedef std::map<wxString, Connection>  ClientMap;

  private:

    TnmsTree_R *         _stree;
    TnmsWxTree *         _tree;
    
    MetricListView *     _mlist;

    ClientMap            _clientMap;

    wxPanel *            _mainPanel;
    wxMenuBar *          _menuBar;
    wxMenu *             _menuFile;

    wxGenericDirCtrl *   _gdir;
    wxListCtrl *         _lCtrl1;
    wxListCtrl *         _lCtrl2;

};


#endif //_CLIENTFRAME_H_

