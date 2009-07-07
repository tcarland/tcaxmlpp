#define _TNMSAUTH_AUTHDCLIENT_CPP_

#include "AuthdClient.h"
#include "AuthDbThread.h"



namespace tnmsauth {


AuthdClient::AuthdClient ( BufferedSocket * sock, AuthDbThread * auth_db ) 
    : TnmsClient(NULL, NULL, sock, false),
      _authdb(auth_db)
{
}


AuthdClient::~AuthdClient() {}


void
AuthdClient::AuthRequestHAndler ( const TnmsAuthRequest & request ) 
{}



} // namespace


//  _TNMSAUTH_AUTHDCLIENT_CPP_

