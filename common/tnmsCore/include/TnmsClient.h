#ifndef _TNMSD_TNMSCLIENT_H_
#define _TNMSD_TNMSCLIENT_H_

#include <set>

#include "TnmsTree.h"
#include "TnmsSocket.h"


namespace tnmsCore {

class AuthClient;


class TnmsClient : public TnmsSocket {

  public:

    TnmsClient ( TnmsTree * tree = NULL );
    TnmsClient ( TnmsTree * tree, AuthClient * auth,
                 tcanetpp::BufferedSocket * sock, bool isAgent = false );

    virtual ~TnmsClient();

    // TnmsSocket
    virtual int     send();
    virtual void    close();

    // TnmsClient
    virtual void    AuthReplyHandler    ( const TnmsAuthReply   & reply );
    virtual void    AuthRequestHandler  ( const TnmsAuthRequest & request );

    void            queueAdd     ( TnmsTree::Node  * node );
    void            queueUpdate  ( TnmsTree::Node  * node );
    void            queueRemove  ( TnmsTree::Node  * node );

    bool            isAgent()  const;
    bool            isMirror() const;
    bool            isMirrorClient() const { return this->isMirror(); }
    
    bool            inTreeSend() const;
    void            inTreeSend   ( bool insend );

    const
    std::string&    getConfig() const { return _xmlConfig; }


  public:

    typedef std::set<TnmsTree::Node*>  UpdateSet;
    typedef std::set<std::string>      RemoveSet;


  protected:

    TnmsTree*            _tree;

    AuthClient*          _auth;
    UpdateSet            _adds;
    UpdateSet            _updates;
    RemoveSet            _removes;

    std::string          _xmlConfig;

    bool                 _isAgent;
    bool                 _isMirror;
    bool                 _authorized;
    bool                 _inTreeSend;
};


} // namespace

#endif  //  _TNMSD_TNMSCLIENT_H_

