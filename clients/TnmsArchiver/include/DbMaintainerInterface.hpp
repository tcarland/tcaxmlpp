#ifndef _TNMSDB_DBMAINTAINERINTERFACE_HPP_
#define _TNMSDB_DBMAINTAINERINTERFACE_HPP_

#include <string>

#include "DbTimePeriod.hpp"

namespace tnmsdb {


class DbMaintainerInterface {
  public:
    virtual ~DbMaintainerInterface() {}

    virtual std::string  getTargetTable       ( const time_t & timestamp ) = 0;
    virtual DbTimePeriod getTargetTimePeriod  ( const time_t & timestamp ) = 0;

    virtual void         getTimePeriods       ( DbNameList   & nameList ) = 0;

    virtual void         createTimePeriods    ( DbIndexList  & indices ) = 0;
    virtual void         deleteTimePeriods    ( DbIndexList  & indices ) = 0;

};


}  // namespace 

#endif // _TNMSDB_DBMAINTAINERINTERFACE_HPP_

