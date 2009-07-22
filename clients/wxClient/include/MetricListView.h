#ifndef _METRICLISTVIEW_H_
#define _METRICLISTVIEW_H_

#include <wx/listctrl.h>

#include "ClientSubscriber.h"


#define TNMS_ID_MLIST  7005


class MetricListView : public wxListCtrl {

  public:

    MetricListView ( wxWindow * parent, const wxWindowID   id,
                                        const wxPoint    & pos,
                                        const wxSize     & size,
                                        long               style);

    virtual ~MetricListView();


    void        Init();

    void        AddDefaultItem();
    void        AddMetricItem   ( TnmsMetric * metric );


  private:

    long        _lastid;
};


#endif  // _METRICLISTVIEW_H_

