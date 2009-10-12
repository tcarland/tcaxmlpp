#ifndef _TNMSDB_DBMAINTAINERINTERFACE_HPP_
#define _TNMSDB_DBMAINTAINERINTERFACE_HPP_

#include <string>

#include "DbTimePeriod.hpp"


namespace tnmsdb {


class DbMaintainerInterface {

  public:

    virtual DbMaintainerInterface() {}


    virtual std::string  getTargetTable       ( const time_t & timestamp ) = 0;
    virtual DbTimePeriod getTargetTimePeriod  ( const time_t & timestamp ) = 0;

    virtual void         getTimePeriods       ( NameList     & nameList ) = 0;

    virtual void         createTimePeriods    ( IndexList    & indices ) = 0;
    virtual void         deleteTimePeriods    ( IndexList    & indices ) = 0;

};

}  // namespace 

#endif // _TNMSDB_DBMAINTAINERINTERFACE_HPP_

