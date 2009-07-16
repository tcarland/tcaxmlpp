#ifndef _TNMSARCHIVE_DBMAINTAINERINTERFACE_HPP_
#define _TNMSARCHIVE_DBMAINTAINERINTERFACE_HPP_

#include <string>

#include "ArchiveTimePeriod.hpp"


namespace tnmsarchive {


class DbMaintainerInterface {

  public:

    virtual DbMaintainerInterface() {}


    virtual std::string  getTargetTable       ( const time_t & timestamp ) = 0;
    virtual TimePeriod   getTargetTimePeriod  ( const time_t & timestamp ) = 0;

    virtual void         getTimePeriods       ( NameList & nameList ) = 0;

    virtual void         createTimePeriods    ( IndexList & indices ) = 0;
    virtual void         deleteTimePeriods    ( IndexList & indices ) = 0;

};

}  // namespace 

#endif // _TNMSARCHIVE_DBMAINTAINERINTERFACE_HPP_

