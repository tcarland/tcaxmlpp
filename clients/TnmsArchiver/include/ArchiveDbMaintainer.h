#ifndef _TNMSDB_DBMAINTAINER_H_
#define _TNMSDB_DBMAINTAINER_H_

#include "DbMaintainerInterface.hpp"


namespace tnmsdb {


class DbMaintainer : public DbMaintainerInterface {

  public:

    DbMaintainer ( const std::string & index_table,
                   const std::string & data_table,
                   int   days_per_interval = 1,
                   int   table_count       = 30 );

    virtual ~DbMaintainer();


    void     runMaintainer();


    /*  DbMaintainerInterface */

    virtual std::string  getTargetTable       ( const time_t & timestamp );
    virtual DbTimePeriod getTargetTimePeriod  ( const time_t & timestamp );

    virtual void         getTimePeriods       ( NameList     & nameList ) = 0;
    virtual void         createTimePeriods    ( IndexList    & indices ) = 0;
    virtual void         deleteTimePeriods    ( IndexList    & indices ) = 0;

    virtual DbTimePeriod getInterval          ( const time_t & timestamp );

  protected:

    void                 slideByDays          ( DbTimePeriod & period, int days );

    DbTimePeriod         getCurrentBegin      ( const time_t & timestamp );
    DbTimePeriod         getCurrentEnd        ( const time_t & timestamp );

    template<typename OutputIterator_>
    void                 getCurrentPeriods    ( OutputIterator_ out );
    template<typename OutputIterator_>
    void                 getDatabasePeriods   ( OutputIterator_ out );

    template<typename OutputIterator_>
    void                 getIntervalsToCreate ( OutputIterator_ out );
    template<typename OutputIterator_>
    void                 getIntervalsToDelete ( OutputIterator_ out );

    template<typename OutputIterator_>
    void                 getPeriodsInRange    ( const DbTimePeriod & begin,
                                                const DbTimePeriod & end,
                                                OutputIterator_     out );

  protected:

    std::string         _indexName;
    std::string         _dataName;
    int                 _numDays;
    int                 _numTables;

    IndexList           _current;
    IndexList           _dbperiods;

};

}  // namespace 

#endif  //  _TNMSDB_DBMAINTAINER_H_
