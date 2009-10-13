#ifndef _TNMSDB_ARCHIVEMAINTAINER_H_
#define _TNMSDB_ARCHIVEMAINTAINER_H_

#include "ArchiveDbInterface.hpp"


namespace tnmsdb {


class ArchiveDbMaintainer : public DbMaintainerInterface {

  public:

    ArchiveDbMaintainer ( const std::string & index_table,
                          const std::string & data_table,
                          int   days_per_interval = 1,
                          int   table_count       = 30 );

    virtual ~ArchiveDbMaintainer();


    void     run();


    /*  ArchiverDbInterface */

    virtual std::string  getTargetTable       ( const time_t & timestamp );
    virtual DbTimePeriod getTargetTimePeriod  ( const time_t & timestamp );

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

#endif  //  _TNMSDB_ARCHIVEMAINTAINER_H_
