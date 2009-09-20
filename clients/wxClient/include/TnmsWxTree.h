#ifndef _TNMSWXTREE_H_
#define _TNMSWXTREE_H_

#include <list>

#include <wx/imaglist.h>
#include <wx/treectrl.h>

#include "TnmsTree_R.hpp"


class TnmsWxTreeItem : public wxTreeItemData {

  public:

    TnmsWxTreeItem ( const wxString & absoluteName, 
                     const wxString & name, 
                     bool  isParent );

    virtual ~TnmsWxTreeItem() {}

    bool        isParent() const;
    bool        hasChildren() const;
    bool        isExpanded() const;

  public:

    TnmsMetric           metric;

    std::list<wxString>  children;
    wxString             absName;
    wxString             name;
    bool                 isparent;
    bool                 isexpanded;

};


#define TNMS_ID_TREE     7000
#define TNMS_ID_WXTREE   7001
#define TNMS_ID_CONNECT  7010
#define TNMS_ID_DISCONN  7011



class TnmsWxTree : public wxControl {

  public:

    TnmsWxTree();

    TnmsWxTree ( wxWindow        * parent,
                 const wxWindowID  id    = wxID_ANY,
                 const wxString  & name  = wxT("TnmsWxTree"),
                 const wxPoint   & pos   = wxDefaultPosition,
                 const wxSize    & size  = wxDefaultSize,
                 long              style = wxSUNKEN_BORDER );

    virtual ~TnmsWxTree();


    bool          Create ( wxWindow        * parent,
                           const wxWindowID  id    = wxID_ANY,
                           const wxString  & name  = wxT("TnmsWxTree"),
                           const wxPoint   & pos   = wxDefaultPosition,
                           const wxSize    & size  = wxDefaultSize,
                           long              style = wxSUNKEN_BORDER );


    void          Init();


    void          OnSize          ( wxSizeEvent     & event );

    void          OnSelect        ( wxTreeEvent & event );
    void          OnDelete        ( wxTreeEvent & event );
    void          OnExpandItem    ( wxTreeEvent & event );
    void          OnCollapseItem  ( wxTreeEvent & event );
    void          OnContext       ( wxTreeEvent & event );


    wxTreeCtrl*   GetTreeCtrl()   { return _treeCtrl; }

    TnmsWxTreeItem* GetItemData   ( wxTreeItemId     id );

    wxTreeItemId  FindChild       ( wxTreeItemId     parentId, 
                                    const wxString & path,
                                    bool             done );

    void          Collapse        ( wxTreeItemId     parentId );
    void          Expand          ( wxTreeItemId     parentId );

    void          DoResize();



    void          SyncTree();


  public:

    void          SetTnmsTree     ( TnmsTree_R * stree );
    TnmsTree_R*   GetTnmsTree();

  protected:

    wxTreeItemId  RecursiveAdd    ( TnmsTree::Node * node, bool parent );
    bool          RecursiveDelete ( wxTreeItemId & id );
  
  public:

    typedef std::map<wxString, wxTreeItemId>  TreeItemMap;


  private:

    TnmsTree_R *        _stree;

    wxTreeCtrl *        _treeCtrl;

    wxTreeItemId        _rootId;

    TreeItemMap         _visible;
    TreeItemMap         _roots;

};


#endif

