#define _TNMSAUTH_AUTHDCLIENT_CPP_

#include "AuthdClient.h"
#include "AuthDbThread.h"

#include "CidrUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsauth {


AuthdClient::AuthdClient ( BufferedSocket * sock, AuthDbThread * auth_db ) 
    : TnmsClient(NULL, NULL, sock, false),
      _authdb(auth_db)
{
}


AuthdClient::~AuthdClient() {}


/** AuthRequestHandler
  *   
 **/
void
AuthdClient::AuthRequestHandler ( const TnmsAuthRequest & request ) 
{
    std::string    user, pass, ipaddr;
    TnmsAuthReply  reply(request.getElementName());

    std::string::size_type  at;

    user = request.getElementName();
    at   = user.find_first_of(':', 0);
    
    if ( at != std::string::npos )
        user = user.substr(0, at);
    
    at   = user.find_first_of('@', 0);
    if ( at != std::string::npos ) {
        ipaddr = user.substr(at+1);
        user   = user.substr(0, at);
    }

    if ( ipaddr.empty() )
        ipaddr = CidrUtils::ntop(request.ipaddr());

    LogFacility::LogMessage("AuthdClient::AuthRequestHandler() for "
        + request.getElementName());

    _authdb->isAuthentic(user, request.agent_key(), ipaddr, reply);
    this->sendMessage(&reply);

    return;
}



} // namespace


//  _TNMSAUTH_AUTHDCLIENT_CPP_

