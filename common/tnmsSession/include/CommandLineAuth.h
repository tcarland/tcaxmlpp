#ifndef _TNMSSESSION_COMMANDLINEAUTH_H_
#define _TNMSSESSION_COMMANDLINEAUTH_H_

#include <string>

#include "AuthenticationInterface.hpp"


namespace tnmsSession {


class CommandLineAuth : public AuthenticationInterface {

  public:

    CommandLineAuth ( const std::string & cmdline );

    virtual ~CommandLineAuth();


    EAuthResult  authenticate ( const std::string & username,
                                const std::string & password );

    const char*  getErrorStr() const;

  private:

    std::string         _cmdLine;
    std::string         _errStr;

};

}  // namespace

#endif  //  _TNMSSESSION_COMMANDLINEAUTH_H_
