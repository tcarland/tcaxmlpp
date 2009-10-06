#ifndef _TNMSD_TNMSCLIENT_H_
#define _TNMSD_TNMSCLIENT_H_

#include <set>

#include "TnmsSocket.h"
#include "TnmsTree.h"


namespace tnmsCore {

class TnmsSubscriber;
class AuthClient;


class TnmsClient : public TnmsSocket {

  public:

    TnmsClient ( TnmsTree * tree = NULL );
    TnmsClient ( TnmsTree * tree, AuthClient * auth,
                 tcanetpp::BufferedSocket * sock, bool isAgent = false );

    virtual ~TnmsClient();

    // TnmsSocket
    virtual int     send ( const time_t & now );
    virtual void    close();
    virtual bool    login ( const std::string & name, const std::string & pw );

    // TnmsClient
    virtual void    AuthReplyHandler    ( const TnmsAuthReply   & reply );
    virtual void    AuthRequestHandler  ( const TnmsAuthRequest & request );

    bool            login();

    bool            isAgent()  const;
    bool            isMirror() const;
    bool            isMirrorClient() const { return this->isMirror(); }
    
    bool            inTreeSend() const;
    void            inTreeSend   ( bool insend );

    const
    std::string&    getConfig() const { return _xmlConfig; }

    void            setSubscriber ( TnmsSubscriber * subscriber );
    TnmsSubscriber* getSubscriber();
    TnmsTree*       getTree();


  protected:

    TnmsTree*            _tree;
    TnmsSubscriber*      _treeNotifier;

    AuthClient*          _auth;

    std::string          _xmlConfig;

    bool                 _isAgent;
    bool                 _isMirror;
    bool                 _inTreeSend;
};


} // namespace

#endif  //  _TNMSD_TNMSCLIENT_H_

