#ifndef TestSqlThread_h_
#define TestSqlThread_h_

#include <string>

#include "Thread.h"
using namespace tcanetpp;

#include "SqlDbPool.h"
using namespace tcasql;


namespace sqlplustest {


class TestSqlThread : public Thread {

  public:

    TestSqlThread ( SqlDbPool * pool, const std::string & sqlstr );
    virtual ~TestSqlThread();

    virtual void  run();

  private:

    SqlDbPool*     _pool;
    std::string    _sqlstr;
    
};


} // namespace


#endif  // TestSqlThread_h_
