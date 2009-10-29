#ifndef TestSqlThread_h_
#define TestSqlThread_h_

#include <string>

#include "Thread.h"

#include "SqlDbPool.h"


namespace sqlplustest {


class TestSqlThread : public tcanetpp::Thread {

  public:

    TestSqlThread ( tcasqlpp::SqlDbPool * pool, const std::string & sqlstr );
    virtual ~TestSqlThread();

    virtual void  run();

  private:

    tcasqlpp::SqlDbPool*    _pool;
    std::string             _sqlstr;
    
};


} // namespace


#endif  // TestSqlThread_h_
