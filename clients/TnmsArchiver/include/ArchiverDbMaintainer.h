#ifndef _TNMSARCHIVE_ARCHIVERDBMAINTAINER_H_
#define _TNMSARCHIVE_ARCHIVERDBMAINTAINER_H_

#include "ArchiverDbInterface.hpp"


namespace tnmsarchive {


class ArchiverDbMaintainer : public ArchiverDbInterface {

  public:

    ArchiverDbMaintainer ( const std::string & index_table,
                           const std::string & data_table,
                           int   days_per_interval = 1,
                           int   table_count = 30 );

    virtual ~ArchiverDbMaintainer();


    void     run();


    /*  ArchiverDbInterface */

    virtual std::string  getTargetTable       ( const time_t & timestamp );
    virtual TimePeriod   getTargetTimePeriod  ( const time_t & timestamp );

    virtual void         getTimePeriods       ( NameList     & nameList );

    virtual void         createTimePeriods    ( IndexList    & indices );
    virtual void         deleteTimePeriods    ( IndexList    & indices );


  protected:

    std::string         _index;
    std::string         _data;
    int                 _numDays;
    int                 _tables;

};

}  // namespace 

#endif  //  _TNMSARCHIVE_ARCHIVERDBMAINTAINER_H_
