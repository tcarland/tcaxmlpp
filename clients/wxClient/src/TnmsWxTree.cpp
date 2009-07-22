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

    _treeCtrl = new wxTreeCtrl(parent, wxID_TREECTRL, pos, size, treeStyle);

    TnmsWxTreeItem * rootData = new TnmsWxTreeItem(wxEmptyString, wxEmptyString, true);
    wxString         rootName = wxT("/");

    _rootId = _treeCtrl->AddRoot(rootName, -1, -1, rootData);
    _treeCtrl->SetItemHasChildren(_rootId);
    
    this->Expand(_rootId);
    SetInitialSize(size);
    DoResize();

    Connect(wxID_TREECTRL, wxEVT_COMMAND_TREE_ITEM_EXPANDED, 
        wxTreeEventHandler(TnmsWxTree::OnExpandItem));
    Connect(wxID_TREECTRL, wxEVT_COMMAND_TREE_ITEM_COLLAPSED,
        wxTreeEventHandler(TnmsWxTree::OnCollapseItem));
    Connect(wxID_TREECTRL, wxEVT_COMMAND_TREE_SEL_CHANGED,
        wxTreeEventHandler(TnmsWxTree::OnSelect));
    Connect(wxID_TREECTRL, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
        wxTreeEventHandler(TnmsWxTree::OnSelect));
    Connect(wxID_TREECTRL, wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
        wxTreeEventHandler(TnmsWxTree::OnContext));
    Connect(wxID_TREECTRL, wxEVT_COMMAND_TREE_DELETE_ITEM,
        wxTreeEventHandler(TnmsWxTree::OnDelete));
    Connect(wxID_TREECTRL, wxEVT_SIZE,
        wxSizeEventHandler(TnmsWxTree::OnSize));

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
TnmsWxTree::OnSelect ( wxTreeEvent & event )
{
    wxTreeItemId     id   = event.GetItem();
    TnmsWxTreeItem * data = (TnmsWxTreeItem*) _treeCtrl->GetItemData(id);

    std::string select = StringUtils::wtocstr(data->absName.c_str());
    LogFacility::LogMessage("OnSelect " + select);

    _stree->tree->subscribe(select, _stree->notifier);
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

    TnmsTree::StringSet  troots;
    TnmsTree::StringSet::iterator  sIter;

    _stree->tree->getRootNames(troots);

    if ( troots.size() == 0 )
        _stree->tree->subscribe("*", _stree->notifier);

    TreeItemMap  rootsold = _roots;
    TreeItemMap::iterator    tIter;

    for ( sIter = troots.begin(); sIter != troots.end(); ++sIter )
    {
        wxString  tname  = StringUtils::ctowstr(*sIter);

        tIter  = _roots.find(tname);
        if ( tIter == _roots.end() ) {
            TnmsWxTreeItem * data = new TnmsWxTreeItem(tname, tname, true);
            wxTreeItemId     id   = _treeCtrl->AppendItem(_rootId, tname, -1, -1, data);

            LogFacility::LogMessage("Adding root " + *sIter);
            _treeCtrl->SetItemHasChildren(id);
            //_stree->tree->subscribe((*sIter + "/"), _stree->notifier);

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
TnmsWxTree::addItems ( wxString & absoluteName )
{
    wxString  pname, name;
    int       indx;

    std::wstring delim = L"/";

    TnmsWxTreeItem * data = NULL;
    wxTreeItemId     pid;

    std::list<wxString>            addlist;
    std::list<wxString>::iterator  aIter;
    TreeItemMap::iterator          vIter = _visible.end();

    pname = absoluteName;
    indx  = StringUtils::lastIndexOf(absoluteName.c_str(), delim);

    if ( indx < 0 && absoluteName.length() > 0 )
        addlist.push_back(absoluteName);

    while ( indx > 0 ) {
        pname = pname.substr(0,indx);
        name  = pname.substr(indx+1);

        vIter = _visible.find(wxString(pname));

        if ( vIter == _visible.end() ) {
            indx = StringUtils::lastIndexOf(pname.c_str(), delim);
            addlist.push_back(name);
        } else 
            break;
    }

    pname = wxT("");
    pid   = _rootId;

    if ( vIter != _visible.end() ) {
        pname = vIter->first;
        pid   = vIter->second;
    }

    for ( aIter = addlist.begin(); aIter != addlist.end(); ++aIter )
    {
        if ( ! pname.empty() )
            pname = pname + delim + *aIter;
        else
            pname = *aIter;
        data  = new TnmsWxTreeItem(pname, *aIter, true);
        pid   = _treeCtrl->AppendItem(pid, *aIter, -1, -1, data);
        _treeCtrl->SetItemHasChildren(pid);

        std::string  pstr  = StringUtils::wtocstr(pname.c_str());
        LogFacility::LogMessage("addItems() adding " + pstr);

        _visible[pname] = pid;
    }

    return; 
}


void
TnmsWxTree::SyncTree()
{
    ClientSubscriber * notifier = _stree->notifier;

    //this->SetupRoot();
    if ( _visible.empty() )
        _stree->tree->subscribe("*", _stree->notifier);

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
        wxString         name = StringUtils::ctowstr(node->getAbsoluteName());

        if ( node->getValue().erase ) {
            adds.erase(nIter++);
            continue;
        }

        if ( ! node->getParent() ) {  
            LogFacility::LogMessage("Add has no parent!");
            this->addItems(name);
            adds.erase(nIter++);
            continue;
        }

        name   = StringUtils::ctowstr(node->getParent()->getAbsoluteName());
        vIter  = _visible.find(name);

        if ( vIter != _visible.end() ) 
        {
            pid   = vIter->second;
            data  = new TnmsWxTreeItem(StringUtils::ctowstr(node->getAbsoluteName()), 
                                       StringUtils::ctowstr(node->getName()), true);
            data->metric = node->getValue().metric;
            id    = _treeCtrl->AppendItem(pid, data->name, -1, -1, data);

            LogFacility::LogMessage("Add " + node->getAbsoluteName());

            _visible[data->absName] = id;
        } else {
            this->addItems(name);
        }

        adds.erase(nIter++);
    }

    TreeRemoveSet & removes = notifier->removes;
    for ( rIter = removes.begin(); rIter != removes.end(); ) 
    {
        name = StringUtils::ctowstr(*rIter);

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
        wxString  parentname  = wxT("/");

        LogFacility::LogMessage("Update " + node->getAbsoluteName());

        name   = StringUtils::ctowstr(node->getAbsoluteName());
        vIter  = _visible.find(name);

        if ( vIter == _visible.end() ) 
        {
            if ( node->getParent() )
                parentname = StringUtils::ctowstr(node->getParent()->getAbsoluteName());
            
            vIter = _visible.find(parentname);

            if ( vIter != _visible.end() ) {
                data = new TnmsWxTreeItem(StringUtils::ctowstr(node->getAbsoluteName()), 
                                          StringUtils::ctowstr(node->getName()), true);
                data->metric  = node->getValue().metric;
                _treeCtrl->SetItemHasChildren(vIter->second);
                id    = _treeCtrl->AppendItem(vIter->second, data->name, -1, -1, data);

                _visible[data->absName] = id;
            } else {
                LogFacility::LogMessage("addItems " + node->getAbsoluteName());
                this->addItems(name);
            }
        } 
        else 
        {
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
}


TnmsTree_R*
TnmsWxTree::GetTnmsTree()
{
    return _stree;
}

// _TNMSWXTREE_CPP_

