#ifndef _TNMSAUTH_TNMSAUTHCLIENT_H_
#define _TNMSAUTH_TNMSAUTHCLIENT_H_

#include "TnmsClient.h"


namespace tnmsauth {


class TnmsAuthClient : public tnmsCore::TnmsClient {

  public:

    TnmsAuthClient ( BufferedSocket * sock, TnmsAuthThread * authThread );
    virtual ~TnmsAuthClient();


    virtual void    AuthRequestHandler ( const TnmsAuthRequest & request );

  protected:

    TnmsAuthThread * _authThread;

};


}  // namespace

#endif  //  _TNMSAUTH_TNMSAUTHCLIENT_H_

