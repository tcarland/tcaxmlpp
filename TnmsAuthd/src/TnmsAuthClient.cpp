#define _TNMSAUTH_TNMSAUTHCLIENT_CPP_

#include "TnmsAuthClient.h"


namespace tnmsauth {


TnmsAuthClient::TnmsAuthClient ( BufferedSocket * sock, TnmsAuthThread * authThread ) 
    : TnmsClient(NULL, NULL, sock, false),
      _authThread(authThread)
{
}

TnmsAuthClient::~TnmsAuthClient() {}

void
TnmsAuthClient::AuthRequestHAndler ( const TnmsAuthRequest & request ) 
{}



} // namespace

//  _TNMSAUTH_TNMSAUTHCLIENT_CPP_

