#ifndef _TNMSWXTREE_H_
#define _TNMSWXTREE_H_

#include <list>

#include <wx/imaglist.h>
#include <wx/treectrl.h>



class TnmsWxTreeItem : public wxTreeItemData {

  public:

    TnmsWxTreeItem ( const wxString & absoluteName, 
                     const wxString & name, 
                     bool isParent );

    virtual ~TnmsWxTreeItem() {}

    bool        isParent() const;
    bool        hasChildren() const;
    bool        isExpanded() const;

  public:

    //TnmsMetric  metric;

    std::list<wxString>  _children;
    wxString             _absName;
    wxString             _name;
    bool                 _isParent;
    bool                 _isExpanded;

};

#define wxID_TREECTRL  7000



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


    void          OnSize          ( wxSizeEvent & event );
    void          OnExpandItem    ( wxTreeEvent & event );
    void          OnCollapseItem  ( wxTreeEvent & event );


    wxTreeCtrl*   GetTreeCtrl()   { return _treeCtrl; }

    wxTreeItemId  FindChild       ( wxTreeItemId     parentId, 
                                    const wxString & path,
                                    bool             done );


    void          Collapse        ( wxTreeItemId     parentId );
    void          Expand          ( wxTreeItemId     parentId );
    void          DoResize();
    void          SetupRoot();

  private:

    wxTreeCtrl *        _treeCtrl;
    wxTreeItemId        _rootId;

    DECLARE_EVENT_TABLE()
};


#endif

