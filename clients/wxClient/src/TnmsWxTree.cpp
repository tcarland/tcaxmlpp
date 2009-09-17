#define _TNMSWXTREE_CPP_

#include <iostream> 

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

    _treeCtrl = new wxTreeCtrl(parent, TNMS_ID_WXTREE, pos, size, treeStyle);

    wxString         rootName = wxT("/");
    TnmsWxTreeItem * rootData = new TnmsWxTreeItem(wxEmptyString, rootName, true);

    _rootId = _treeCtrl->AddRoot(rootName, -1, -1, rootData);
    _treeCtrl->SetItemHasChildren(_rootId);
    
    this->Expand(_rootId);
    SetInitialSize(size);
    DoResize();

    Connect(TNMS_ID_TREE, wxEVT_SIZE, wxSizeEventHandler(TnmsWxTree::OnSize));

    return true;
}


TnmsWxTreeItem*
TnmsWxTree::GetItemData ( wxTreeItemId  id )
{
    return( (TnmsWxTreeItem*) _treeCtrl->GetItemData(id) );
}


void
TnmsWxTree::OnExpandItem ( wxTreeEvent & event )
{
    /*
    wxTreeItemId  parentId = event.GetItem();

    if ( ! _rootId.IsOk() )
        _rootId = _treeCtrl->GetRootItem();
    */
    LogFacility::LogMessage("TnmsWxTree::OnExpandItem");

    //return this->Expand(parentId);
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
TnmsWxTree::OnSelect ( wxTreeEvent & event )
{
    wxTreeItemId     id   = event.GetItem();
    TnmsWxTreeItem * data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(id);

    std::string select = StringUtils::wtocstr(data->absName.c_str());
    LogFacility::LogMessage("OnSelect " + select);

    return;
}


void
TnmsWxTree::OnDelete ( wxTreeEvent & event )
{
    LogFacility::LogMessage("OnDelete ");
}


void
TnmsWxTree::OnContext ( wxTreeEvent & event )
{
    wxTreeItemId     id   = event.GetItem();
    TnmsWxTreeItem * data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(id);

    std::string select = StringUtils::wtocstr(data->absName.c_str());
    LogFacility::LogMessage("OnContext " + select);
}


void
TnmsWxTree::Expand ( wxTreeItemId  parentId )
{

}


void
TnmsWxTree::Collapse ( wxTreeItemId  parentId )
{

}


void
TnmsWxTree::DoResize()
{
    wxSize sz = GetClientSize();
    _treeCtrl->SetSize(0, 0, sz.x, sz.y);
}


wxTreeItemId
TnmsWxTree::RecursiveAdd ( TnmsTree::Node * node, bool parent )
{
    wxString               name, aname, pname;
    TreeItemMap::iterator  vIter;
    TnmsWxTreeItem *       data = NULL;
    wxTreeItemId           pid  = _rootId;
    wxTreeItemId           id;

    name  = wxString::FromAscii(node->getName().c_str());
    LogFacility::LogMessage("TnmsWxTree::Add " + node->getAbsoluteName());

    if ( node->getParent() ) 
    {
        pname = wxString::FromAscii(node->getParent()->getAbsoluteName().c_str());
        vIter = _visible.find(pname);

        if ( vIter == _visible.end() )
            pid = this->RecursiveAdd(node->getParent(), true);
        else
            pid = vIter->second;
    }

    aname = wxString::FromAscii(node->getAbsoluteName().c_str());
    data  = new TnmsWxTreeItem(aname, name, true);
    data->metric = node->getValue().metric;
    id    = _treeCtrl->AppendItem(pid, name, -1, -1, data);

    if ( parent )
        _treeCtrl->SetItemHasChildren(id);

    _visible[aname] = id;

    return id;
}

void
TnmsWxTree::SyncTree()
{
    ClientSubscriber * notifier = _stree->notifier;

    if ( _visible.empty() )
        _stree->tree->subscribe("/", _stree->notifier);

    if ( ! notifier->haveUpdates() )
        return;

    if ( ! notifier->trylock() )
        return;

    LogFacility::LogMessage("SyncTree() has updates");

    TreeUpdateSet::iterator  nIter;
    TreeRemoveSet::iterator  rIter;
    TreeItemMap::iterator    vIter;

    wxString         name;
    wxTreeItemId     pid, id;
    TnmsWxTreeItem * data = NULL;

    TreeUpdateSet & adds = notifier->adds;
    for ( nIter = adds.begin(); nIter != adds.end(); )
    {
        TnmsTree::Node * node = *nIter;
        wxString         name = wxString::FromAscii(node->getAbsoluteName().c_str());

        if ( node->getValue().erase ) {
            adds.erase(nIter++);
            continue;
        }

        vIter  = _visible.find(name);
        if ( vIter != _visible.end() ) {
            pid   = vIter->second;
            data  = (TnmsWxTreeItem*) _treeCtrl->GetItemData(pid);
            
            data->metric = node->getValue().metric;

            LogFacility::LogMessage("Add " + node->getAbsoluteName());
        } else {
            this->RecursiveAdd(node, true);
        }

        adds.erase(nIter++);
    }

    TreeRemoveSet & removes = notifier->removes;
    for ( rIter = removes.begin(); rIter != removes.end(); ) 
    {
        name = wxString::FromAscii(rIter->c_str());

        vIter = _visible.find(name);

        if ( vIter != _visible.end() ) {
            _treeCtrl->Delete(vIter->second);
            _visible.erase(vIter);
        }

        removes.erase(rIter++);
    }

    TreeUpdateSet & updates = notifier->updates;
    for ( nIter = updates.begin(); nIter != updates.end(); ) 
    {
        TnmsTree::Node * node = *nIter;

        LogFacility::LogMessage("Update " + node->getAbsoluteName());

        name   = wxString::FromAscii(node->getAbsoluteName().c_str());
        vIter  = _visible.find(name);

        if ( vIter == _visible.end() ) {
            this->RecursiveAdd(node, true);
        } else {
            data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(vIter->second);
            data->metric = node->getValue().metric;
        }

        updates.erase(nIter++);
    }

    _stree->notifier->unlock();

    return;
}


void
TnmsWxTree::SetTnmsTree ( TnmsTree_R * tree )
{
    _stree = tree;

    if ( tree == NULL )
        return;

    if ( _stree->mutex->lock() )
    {
        _stree->tree->subStructure((TnmsSubscriber*) _stree->notifier);
        _stree->mutex->unlock();
    }

    return;
}


TnmsTree_R*
TnmsWxTree::GetTnmsTree()
{
    return _stree;
}

// _TNMSWXTREE_CPP_

