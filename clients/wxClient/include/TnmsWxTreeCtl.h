#ifndef _TNMSCLIENT_TNMSWXTREECTL_H_
#define _TNMSCLIENT_TNMSWXTREECTL_H_

#include <list>
#include <map>

#include <wx/imaglist.h>
#include <wx/treectrl.h>

#include "TnmsTree.h"
using namespace tnmsCore;


namespace tnmsclient {


typedef std::map<std::string, TnmsTree::NotifySet>  SubscriberMap;


#define TNMS_ID_TREE     7000
#define TNMS_ID_WXTREE   7001
#define TNMS_ID_CONNECT  7010
#define TNMS_ID_DISCONN  7011
#define TNMS_ID_VERSION  7050

class ClientTreeMutex;



class TreeItem : public wxTreeItemData {
  public:
    std::string   absoluteName;

    TreeItem ( const std::string & name )
        : absoluteName(name) {}
};



class TnmsWxTreeCtl : public wxControl {
 
  public:

    typedef std::map<std::string, wxTreeItemId>  TreeIdMap;
    typedef std::set<std::string>                StringSet;


  public:

    TnmsWxTreeCtl();

    TnmsWxTreeCtl ( wxWindow        * parent,
                    const wxWindowID  id     = wxID_ANY,
                    const wxString  & name   = wxT("TnmsWxTree"),
                    const wxPoint   & pos    = wxDefaultPosition,
                    const wxSize    & size   = wxDefaultSize,
                    long              style  = wxSUNKEN_BORDER );

    virtual ~TnmsWxTreeCtl();


    bool          Create ( wxWindow        * parent,
                           const wxWindowID  id     = wxID_ANY,
                           const wxString  & name   = wxT("TnmsWxTree"),
                           const wxPoint   & pos    = wxDefaultPosition,
                           const wxSize    & size   = wxDefaultSize,
                           long              style  = wxSUNKEN_BORDER );


    void          Init();


    wxTreeItemId  FindChild       ( wxTreeItemId     parentId, 
                                    const wxString & path,
                                    bool             done );

    void          OnSize          ( wxSizeEvent    & event );
    void          OnSelect        ( wxTreeEvent    & event );
    void          OnDelete        ( wxTreeEvent    & event );

    TreeItem*     GetItemData     ( wxTreeItemId     id );
    TnmsMetric    GetItemMetric   ( wxTreeItemId   & id );


    int           Subscribe       ( const std::string & name, TreeSubscriber * sub );
    int           Unsubscribe     ( const std::string & name, TreeSubscriber * sub );

    void          DoResize();


    void          Sync();


  public:

    void          SetTnmsTree     ( ClientTreeMutex * stree );
    ClientTreeMutex* GetTnmsTree();

    wxTreeCtrl*   GetTreeCtrl()   { return _treeCtrl; }


  protected:

    void          Add             ( TnmsTree::Node * node );
    wxTreeItemId  RecursiveAdd    ( TnmsTree::Node * node );

 
  private:

    ClientTreeMutex *   _mtree;

    wxTreeCtrl *        _treeCtrl;
    wxTreeItemId        _rootId;

    TreeIdMap           _idMap;

    TnmsTree *          _visible;
    SubscriberMap       _subs;

};

} // namespace

#endif // _TNMSCLIENT_TNMSWXTREECTL_H_


