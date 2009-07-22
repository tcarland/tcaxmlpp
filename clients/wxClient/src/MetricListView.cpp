#define _METRICLISTVIEW_CPP_

#include "MetricListView.h"



MetricListView::MetricListView ( wxWindow * parent, const wxWindowID   id,
                                                    const wxPoint    & pos,
                                                    const wxSize     & size,
                                                    long               style )
    : wxListCtrl(parent, id, pos, size, style|wxLC_REPORT),
      _lastid(0)
{
    this->Init();
}

MetricListView::~MetricListView()
{
}


void
MetricListView::Init()
{
    wxListItem  itemCol;

    itemCol.SetText(_T("Name"));
    itemCol.SetImage(-1);
    this->InsertColumn(0, itemCol);

    itemCol.SetText(_T("Value Type"));
    this->InsertColumn(1, itemCol);

    itemCol.SetText(_T("Value"));
    this->InsertColumn(2, itemCol);

    itemCol.SetText(_T("Data"));
    this->InsertColumn(3, itemCol);

    this->AddDefaultItem();

}

void
MetricListView::AddDefaultItem()
{
    wxListItem  mitem;
    long        id;

    id = this->InsertItem(_lastid, _T("/"), 0);
    _lastid++;

    this->SetItemData(id, id);
    this->SetItem(id, 1, _T("NOVALUE"));
    this->SetItem(id, 2, _T(" "));
    this->SetItem(id, 3, _T("Default Entry"));

    mitem.m_itemId = id;
    mitem.SetTextColour(*wxBLUE);
    mitem.SetFont(*wxITALIC_FONT);
    mitem.SetBackgroundColour(*wxLIGHT_GREY);
    
    this->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    this->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    this->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
    this->SetColumnWidth(3, wxLIST_AUTOSIZE);

    return;
}


void
MetricListView::AddMetricItem ( TnmsMetric * metric )
{

}


// _METRICLISTVIEW_CPP_

