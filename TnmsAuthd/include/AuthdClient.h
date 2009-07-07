#ifndef _TNMSAUTH_AUTHDCLIENT_H_
#define _TNMSAUTH_AUTHDCLIENT_H_

#include "TnmsClient.h"


namespace tnmsauth {


class AuthDbThread;



class AuthdClient : public tnmsCore::TnmsClient {

  public:

    AuthdClient ( BufferedSocket * sock, AuthDbThread * auth_db );

    virtual ~AuthdClient();


    virtual void    AuthRequestHandler ( const TnmsAuthRequest & request );


  protected:

    AuthDbThread *   _authdb;

};


}  // namespace

#endif  //  _TNMSAUTH_AUTHDCLIENT_H_

