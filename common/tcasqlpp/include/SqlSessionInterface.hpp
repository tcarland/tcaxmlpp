#ifndef _INCLUDE_SQLSESSIONINTERFACE_HPP_
#define _INCLUDE_SQLSESSIONINTERFACE_HPP_

#include <string>

namespace tcasqlpp {


class SqlSessionInterface {

  public:

    virtual ~SqlSessionInterface() {}

    virtual bool dbconnect() = 0;
    virtual void dbclose() = 0;
    virtual bool isConnected() = 0;
    
    virtual
    std::string  escapeString ( const std::string & str ) = 0;

    virtual
    const std::string&  sqlErrorStr() = 0;
};


} // namespace


#endif  // _INCLUDE_SQLSESSIONINTERFACE_HPP_
