#ifndef _INCLUDE_SQLEXCEPTION_HPP_
#define _INCLUDE_SQLEXCEPTION_HPP_

#include "Exception.hpp"


namespace tcasqlpp {


class SqlException : public tcanetpp::Exception {

  public:

    SqlException ( const std::string & err ) 
        : tcanetpp::Exception(err) {}

    virtual ~SqlException() throw() {}

};

} // namespace

#endif  // _INCLUDE_SQLEXCEPTION_HPP_

