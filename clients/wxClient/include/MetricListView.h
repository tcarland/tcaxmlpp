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


    bool        AddMetricItem   ( TnmsMetric & metric );


  private:

    void        Init();
    void        AddDefaultItem();
    long        getNewId();

  private:

    long        _lastid;
};


#endif  // _METRICLISTVIEW_H_

