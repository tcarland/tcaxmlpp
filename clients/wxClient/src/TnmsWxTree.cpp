#define _TNMSWXTREE_CPP_

#include "TnmsWxTree.h"
#include "ClientSubscriber.h"

#include "LogFacility.h"
using namespace tcanetpp;


// ----------------------------------------------------------------------

TnmsWxTreeItem::TnmsWxTreeItem ( const wxString & absoluteName, 
                                 const wxString & name, 
                                 bool             isParent )
    : absName(absoluteName), 
      name(name), 
      isparent(isParent),
      isexpanded(false)
{}


bool
TnmsWxTreeItem::isParent() const
{
    return isparent;
}

bool
TnmsWxTreeItem::isExpanded() const
{
    return isexpanded;
}


bool
TnmsWxTreeItem::hasChildren() const
{
    return(children.size() > 0);
}


// ----------------------------------------------------------------------

BEGIN_EVENT_TABLE(TnmsWxTree, wxControl)
  EVT_TREE_ITEM_EXPANDING     (wxID_TREECTRL, TnmsWxTree::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSED     (wxID_TREECTRL, TnmsWxTree::OnCollapseItem)
  EVT_SIZE                    (TnmsWxTree::OnSize)
END_EVENT_TABLE()

// EVT_TREE_ITEM_ACTIVATED
// EVT_TREE_ITEM_SEL_CHANGED

// ----------------------------------------------------------------------


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

    long treeStyle  = wxTR_HAS_BUTTONS;
    treeStyle      |= wxTR_HIDE_ROOT;
    treeStyle      |= style;

    _treeCtrl = new wxTreeCtrl(parent, wxID_TREECTRL, pos, size, treeStyle);

    //_treeCtrl->SetImageList

    TnmsWxTreeItem * rootData = new TnmsWxTreeItem(wxEmptyString, wxEmptyString, true);

    wxString  rootName = wxT("/");

    _rootId = _treeCtrl->AddRoot(rootName, -1, -1, rootData);
    _treeCtrl->SetItemHasChildren(_rootId);
    
    this->Expand(_rootId);
    SetInitialSize(size);
    DoResize();

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
TnmsWxTree::OnSize ( wxSizeEvent & WXUNUSED(event) )
{
    this->DoResize();
}

void
TnmsWxTree::Expand ( wxTreeItemId  parentId )
{
    std::string  absoluteName;

    TnmsWxTreeItem * data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(parentId);

    LogFacility::LogMessage("TnmsWxTree::Expand");
    if ( data->isExpanded() )
        return;

    data->isexpanded = true;

    if ( parentId == _treeCtrl->GetRootItem())
    {
        this->SetupRoot();
        return;
    }

    absoluteName = StringUtils::wtocstr(data->absName.c_str());
    _stree->tree->subscribe(absoluteName, (TnmsSubscriber*) _stree->notifier);

    _visible[data->absName] = parentId;

    return;
}


void
TnmsWxTree::Collapse ( wxTreeItemId  parentId )
{
    wxTreeItemId  child;

    TnmsWxTreeItem * data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(parentId);

    if ( ! data->isExpanded() )
        return;

    data->isexpanded = false;

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
    if ( _stree == NULL || _stree->tree == NULL )
        return;

/*
    wxString  itemname = wxT("tcanms");
    TnmsWxTreeItem * data = new TnmsWxTreeItem(itemname, itemname, true);
    wxTreeItemId     id   = _treeCtrl->AppendItem(_rootId, itemname, -1, -1, data);

    _treeCtrl->SetItemHasChildren(id);
    _treeCtrl->SelectItem(_rootId);
    _treeCtrl->EnsureVisible(_rootId);
    _treeCtrl->Expand(_rootId);
*/
    
    //if ( _stree->notifier->haveUpdates() )

    TnmsTree::StringSet  troots;
    TnmsTree::StringSet::iterator  sIter;

    _stree->tree->getRootNames(troots);

    TreeItemMap  rootsold = _roots;
    TreeItemMap::iterator    tIter;

    for ( sIter = troots.begin(); sIter != troots.end(); ++sIter )
    {
        wxString  tname  = StringUtils::ctowstr(*sIter);

        tIter  = _roots.find(tname);
        if ( tIter == _roots.end() ) {
            TnmsWxTreeItem * data = new TnmsWxTreeItem(tname, tname, true);
            wxTreeItemId     id   = _treeCtrl->AppendItem(_rootId, tname, -1, -1, data);

            _treeCtrl->SetItemHasChildren(id);

            _roots[tname] = id;
            _visible[tname] = id;
        } else {
            rootsold.erase(tname);
            _visible.erase(tname);
        }
    }

    for ( tIter = rootsold.begin(); tIter != rootsold.end(); ++tIter )
        _treeCtrl->Delete(tIter->second);


    return;
}


void
TnmsWxTree::DoResize()
{
    wxSize sz = GetClientSize();
    _treeCtrl->SetSize(0, 0, sz.x, sz.y);
}

void
TnmsWxTree::SyncTree()
{
    ClientSubscriber * notifier = _stree->notifier;

    this->SetupRoot();

    if ( ! notifier->haveUpdates() )
        return;

    if ( ! notifier->trylock() )
        return;

    TreeUpdateSet::iterator  nIter;
    TreeRemoveSet::iterator  rIter;
    TreeItemMap::iterator    vIter;

    wxString         name, parentname;
    wxTreeItemId     pid, id;
    TnmsWxTreeItem * data = NULL;

    TreeUpdateSet & adds = notifier->adds;
    for ( nIter = adds.begin(); nIter != adds.end(); )
    {
        TnmsTree::Node * node = *nIter;

        if ( node->getValue().erase ) {
            adds.erase(nIter++);
            continue;
        }

        parentname = StringUtils::ctowstr(node->getParent()->getAbsoluteName());

        vIter = _visible.find(parentname);

        if ( vIter != _visible.end() ) 
        {
            pid   = vIter->second;
            data  = new TnmsWxTreeItem(StringUtils::ctowstr(node->getAbsoluteName()), 
                                       StringUtils::ctowstr(node->getName()), true);
            data->metric = node->getValue().metric;
            id    = _treeCtrl->AppendItem(pid, data->name, -1, -1, data);

            _visible[data->name] = id;
        }
        adds.erase(nIter++);
    }

    TreeRemoveSet & removes = notifier->removes;
    for ( rIter = removes.begin(); rIter != removes.end(); ) 
    {
        name = StringUtils::ctowstr(*rIter);

        vIter = _visible.find(name);

        if ( vIter != _visible.end() )
            _treeCtrl->Delete(vIter->second);

        removes.erase(rIter++);
    }

    TreeUpdateSet & updates = notifier->updates;
    for ( nIter = updates.begin(); nIter != updates.end(); ) 
    {
        TnmsTree::Node * node = *nIter;

        name  = StringUtils::ctowstr(node->getAbsoluteName());
        vIter = _visible.find(name);

        if ( vIter != _visible.end() )
        {
            data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(vIter->second);
            data->metric = node->getValue().metric;
        }

        updates.erase(nIter++);
    }

    return;
}


void
TnmsWxTree::SetTnmsTree ( TnmsTree_R * tree )
{
    _stree = tree;
}

TnmsTree_R*
TnmsWxTree::GetTnmsTree()
{
    return _stree;
}

// _TNMSWXTREE_CPP_

