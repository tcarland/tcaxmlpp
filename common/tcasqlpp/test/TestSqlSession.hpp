#ifndef _INCLUDE_TESTSQLSESSION_HPP_
#define _INCLUDE_TESTSQLSESSION_HPP_


#if defined(USE_MYSQL)
#  include "TestMysqlSession.h"
#elif defined(USE_POSTGRESQL)
#  include "TestPgsqlSession.h"
#endif


namespace sqlplustest {

#if defined(USE_MYSQL)
  typedef TestMysqlSession TestSqlSession;
#elif defined(USE_POSTGRESQL)
  typedef TestPgsqlSession TestSqlSession;
#endif


} // namespace

#endif  /*  _INCLUDE_TESTSQLSESSION_HPP_  */

