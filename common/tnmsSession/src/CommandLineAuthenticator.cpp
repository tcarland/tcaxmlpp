#define _TNMSSESSION_COMMANDLINEAUTHENTICATOR_CPP_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


#include "CommandLineAuthenticator.h"



namespace tnmsSession {



CommandLineAuthenticator::CommandLineAuthenticator ( const std::string & cmdline )
    : _cmdLine(cmdline)
{
    
}

CommandLineAuthenticator::~CommandLineAuthenticator()
{ }


AuthenticationInterface::EAuthResult
CommandLineAuthenticator::authenticate ( const std::string & username, const std::string & password )
{
    const char * cfmt  = "%s -q -u %s";
    const char * fmt   = "%s";
    const char * mode  = "w";
    char cmdbuf[512]   = { 0 };
    EAuthResult eRes   = AUTH_FAILURE;
    FILE *      file   = NULL;
    
    ::snprintf(cmdbuf, sizeof(cmdbuf), cfmt, this->_cmdLine.c_str(), username.c_str());
    file  = ::popen(cmdbuf, mode);
    ::snprintf(cmdbuf, sizeof(cmdbuf), fmt, password.c_str());
    
    if ( file ) 
    {
        ::fprintf(file, fmt, cmdbuf);
        int s = ::pclose(file);
        
        if ( s == 0 ) {
            eRes    = AUTH_SUCCESS;
        } else {
            _errStr = "Shell cmd exited with non-zero value";
            eRes    = AUTH_FAILURE;
        }
    }
    else 
    {
        eRes = AUTH_FAILURE;
        switch ( errno ) 
        {
            case EAGAIN:
                break;
            case ENOMEM:
                _errStr = "Call to fork() failed";
                break;
            case EFAULT:
                break;
            case EMFILE:
                _errStr = "Call to pipe() failed";
                break;
            default:
                _errStr = "Unknown failure";
                break;
        }
    }
    
    return eRes;
}


const std::string&
CommandLineAuthenticator::getErrorStr() const 
{
    return _errStr.c_str();
}


}  // namespace

//  _TNMSSESSION_COMMANDLINEAUTHENTICATOR_CPP_

