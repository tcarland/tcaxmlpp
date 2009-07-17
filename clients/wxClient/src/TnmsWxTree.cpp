

#include "TnmsWxTree.h"


#include "LogFacility.h"
using namespace tcanetpp;



TnmsWxTreeItem::TnmsWxTreeItem ( const wxString & absoluteName, 
                     const wxString & name, 
                     bool isParent )
    : _absName(absoluteName), _name(name), _isParent(isParent)
{}


bool
TnmsWxTreeItem::isParent() const
{
    return _isParent;
}

bool
TnmsWxTreeItem::isExpanded() const
{
    return _isExpanded;
}


bool
TnmsWxTreeItem::hasChildren() const
{
    return(_children.size() > 0);
}



BEGIN_EVENT_TABLE(TnmsWxTree, wxControl)
  EVT_TREE_ITEM_EXPANDING     (wxID_TREECTRL, TnmsWxTree::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSED     (wxID_TREECTRL, TnmsWxTree::OnCollapseItem)
END_EVENT_TABLE()



TnmsWxTree::TnmsWxTree()
{
    this->Init();
}

TnmsWxTree::TnmsWxTree ( wxWindow        * parent,
                         const wxWindowID  id,
                         const wxString  & name,
                         const wxPoint   & pos,
                         const wxSize    & size,
                         long              style )
{
    this->Init();
    this->Create(parent, id, name, pos, size, style);
}


TnmsWxTree::~TnmsWxTree()
{
    delete _treeCtrl;
}


void
TnmsWxTree::Init()
{
    _rootId = 0;
}


bool
TnmsWxTree::Create  ( wxWindow        * parent,
                      const wxWindowID  id,
                      const wxString  & name,
                      const wxPoint   & pos,
                      const wxSize    & size,
                      long              style )
{
    if ( ! wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    long treeStyle = wxTR_HAS_BUTTONS;
    treeStyle |= style;

    _treeCtrl = new wxTreeCtrl(parent, wxID_TREECTRL, pos, size, treeStyle);

    //_treeCtrl->SetImageList

    TnmsWxTreeItem * rootData = new TnmsWxTreeItem(wxEmptyString, wxEmptyString, true);

    wxString  rootName = wxT("/");

    _rootId = _treeCtrl->AddRoot(rootName, 3, -1, rootData);
    _treeCtrl->SetItemHasChildren(_rootId);

    return true;
}


void
TnmsWxTree::OnExpandItem ( wxTreeEvent & event )
{
    wxTreeItemId  parentId = event.GetItem();

    if ( ! _rootId.IsOk() )
        _rootId = _treeCtrl->GetRootItem();

    LogFacility::LogMessage("TnmsWxTree::OnExpandItem");

    return this->Expand(parentId);
}


void
TnmsWxTree::OnCollapseItem ( wxTreeEvent & event )
{
    LogFacility::LogMessage("TnmsWxTree::OnCollapseItem");
    return this->Collapse(event.GetItem());
}

void
TnmsWxTree::Expand ( wxTreeItemId  parentId )
{
    TnmsWxTreeItem * data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(parentId);

    if ( data->isExpanded() )
        return;

    data->_isExpanded = true;

    if ( parentId == _treeCtrl->GetRootItem())
    {
        this->SetupRoot();
        return;
    }

}


void
TnmsWxTree::Collapse ( wxTreeItemId  parentId )
{
    wxTreeItemId  child;

    TnmsWxTreeItem * data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(parentId);

    if ( ! data->isExpanded() )
        return;

    data->_isExpanded = false;

    wxTreeItemIdValue  cookie;

    child = _treeCtrl->GetFirstChild(parentId, cookie);
    while ( child.IsOk() )
    {
        _treeCtrl->Delete(child);
        child = _treeCtrl->GetFirstChild(parentId, cookie);
    }

    if ( parentId != _treeCtrl->GetRootItem())
        _treeCtrl->Collapse(parentId);

}

void
TnmsWxTree::SetupRoot()
{
    return;
}



