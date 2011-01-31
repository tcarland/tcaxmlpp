#ifndef _TNMSWXTREE_H_
#define _TNMSWXTREE_H_

#include <list>

#include <wx/imaglist.h>
#include <wx/treectrl.h>


#include "TnmsTree_R.hpp"


namespace tnmsclient {


typedef std::map<std::string, TnmsTree::NotifySet>  SubscriberMap;


#define TNMS_ID_TREE     7000
#define TNMS_ID_WXTREE   7001
#define TNMS_ID_CONNECT  7010
#define TNMS_ID_DISCONN  7011
#define TNMS_ID_VERSION  7050



class TreeItem : public wxTreeItemData {

  public:

    std::string   absoluteName;

    TreeItem ( const std::string & name )
        : absoluteName(name) {}
 
};



class TnmsWxTree : public wxControl {
 
  public:

    typedef std::map<std::string, wxTreeItemId>  TreeIdMap;
    typedef std::set<std::string>                StringSet;


  public:

    TnmsWxTree();

    TnmsWxTree ( wxWindow        * parent,
                 const wxWindowID  id     = wxID_ANY,
                 const wxString  & name   = wxT("TnmsWxTree"),
                 const wxPoint   & pos    = wxDefaultPosition,
                 const wxSize    & size   = wxDefaultSize,
                 long              style  = wxSUNKEN_BORDER );

    virtual ~TnmsWxTree();


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

    void          SetTnmsTree     ( TnmsTree_R * stree );
    TnmsTree_R*   GetTnmsTree();

    wxTreeCtrl*   GetTreeCtrl()   { return _treeCtrl; }


  protected:

    void          Add             ( TnmsTree::Node * node );
    wxTreeItemId  RecursiveAdd    ( TnmsTree::Node * node );

 
  private:

    TnmsTree_R *        _stree;

    wxTreeCtrl *        _treeCtrl;
    wxTreeItemId        _rootId;

    TreeIdMap           _idMap;

    TnmsTree *          _visible;
    SubscriberMap       _subs;

};

} // namespace

#endif  // _TNMSWXTREE_H_

