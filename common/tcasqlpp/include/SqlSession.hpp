#ifndef _INCLUDE_SQLSESSION_HPP_
#define _INCLUDE_SQLSESSION_HPP_


#define  TCASQLPLUS_VERSION  "0.117"

#include "SqlFactoryInterface.hpp"


#if defined(USE_MYSQL)
#  include "MysqlSession.h"
#elif defined(USE_POSTGRESQL)
#  include "PgsqlSession.h"
#endif


namespace tcasql {

#if defined(USE_MYSQL)
  typedef MysqlSession SqlSession;
#elif defined(USE_POSTGRESQL)
  typedef PgsqlSession SqlSession;
#endif


} // namespace


#endif  /*  _INCLUDE_SQLSESSION_HPP_  */
