#ifndef _TNMSSESSION_COMMANDLINEAUTHENTICATOR_H_
#define _TNMSSESSION_COMMANDLINEAUTHENTICATOR_H_

#include <string>

#include "AuthenticationInterface.hpp"


namespace tnmsSession {


class CommandLineAuthenticator : public AuthenticationInterface {

  public:

    CommandLineAuthenticator ( const std::string & cmdline );

    virtual ~CommandLineAuthenticator();


    EAuthResult  authenticate ( const std::string & username,
                                const std::string & password );


    const
    std::string& getErrorStr() const;


  private:

    std::string         _cmdLine;
    std::string         _errStr;

};


}  // namespace

#endif  //  _TNMSSESSION_COMMANDLINEAUTHENTICATOR_H_

