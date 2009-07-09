#ifndef  _AUTHENTICATIONINTERFACE_HPP_
#define  _AUTHENTICATIONINTERFACE_HPP_

#include <string>


namespace tnmsSession {


class AuthenticationInterface {

  public:

    enum EAuthResult 
    {
        AUTH_SUCCESS,
        AUTH_FAILURE
    };


    virtual ~AuthenticationInterface() {}


    virtual EAuthResult authenticate ( const std::string & username,
                                       const std::string & password ) = 0;
 

};


} // namespace

#endif //  _AUTHENTICATIONINTERFACE_HPP_

