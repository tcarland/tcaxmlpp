#ifndef _TNMSSOCKET_H_
#define _TNMSSOCKET_H_

#include <sstream>

#include "BufferedSocket.h"
using namespace tcanetpp;

#include "tnmsProtocol.h"
#include "TnmsFilters.hpp"

#include "zipstream.hpp"
using namespace zlib_stream;



namespace tnmscore {


class TnmsSocket {

public:

    TnmsSocket  ( uint32_t  flush_limit = DEFAULT_FLUSH_LIMIT );

    TnmsSocket  ( ipv4addr_t ip, uint16_t port,
                  uint32_t  flush_limit = DEFAULT_FLUSH_LIMIT );

    TnmsSocket  ( tcanetpp::BufferedSocket * sock,
                  uint32_t  flush_limit = DEFAULT_FLUSH_LIMIT );

    virtual ~TnmsSocket();


    int                 openConnection  ( const std::string & host,
                                          uint16_t port );
    int                 openConnection();
    int                 closeConnection();


    virtual int         connect()  { return this->openConnection(); }
    virtual int         close()    { return this->closeConnection(); }


    int                 send();
    int                 receive();

    ssize_t             flush();
    size_t              txBytesBuffered();


    /*  Attributes  */

    bool                isConnected()  const;
    bool                isConnecting() const;

    const sock_t&       getDescriptor() const;
    const sock_t&       getSockFD() const    { return this->getSocketDescriptor(); }

    void                reconnectTime        ( const time_t & secs );
    const time_t&       reconnectTime() const;

    void                compression          ( bool    compress );
    bool                compression() const;

    void                setTxQueueSize       ( size_t  bytes );
    size_t              getTxQueueSize() const;

    std::string         getClientLogin() const;
    std::string         getHostStr() const;
    std::string         getAddrStr() const;
    std::string         getHostname() const;
    uint16_t            getHostPort() const;
    uint32_t            getVersion() const;

    std::string         getErrorStr() const;
    void                setDebug            ( bool debug );


    /*  Additional attributes */

    void                enableFlush         ( bool  flush );
    bool                flushEnabled();
    void                flushLimit          ( uint32_t limit );
    uint32_t            flushLimit();

    void                enableTimeout       ( bool   timeout );
    void                setTimeout          ( time_t secs );
    time_t              getTimeout();


    /*  Subscriptions  */

    void                login                ( const std::string &  user,
                                               const std::string &  pw );

    bool                isAuthorized() const;
    bool                isSubscribed() const;

    virtual bool        subscribeStructure();
    virtual bool        unsubscribeStructure();

    virtual bool        subscribeAll();
    virtual bool        unsubscribeAll();

    virtual bool        subscribe           ( const TnmsOid  & oid );
    virtual bool        unsubscribe         ( const TnmsOid  & oid );

    virtual bool        subscribeLevel      ( const TnmsOid  & oid );
    virtual bool        unsubscribeLevel    ( const TnmsOid  & oid );

    int                 subscribeCount();
    void                resubscribe();


    /*  Messaging Callbacks  */

    virtual void        AddHandler          ( const TnmsAdd     & addmsg ) {}
    virtual void        RemoveHandler       ( const TnmsRemove  & remmsg ) {}
    virtual void        MetricHandler       ( const TnmsMetric  & metric ) {}
    virtual void        RequestHandler      ( const TnmsRequest & request ) {}

    virtual void        SubscribeHandler    ( const std::string & name ) {}
    virtual void        UnsubscribeHandler  ( const std::string & name ) {}
    virtual void        StructureHandler    ( bool  subscribe ) {}

    virtual void        LastRecordHandler() {}

    virtual void        AuthRequestHandler  ( const TnmsAuthRequest & request );
    virtual void        AuthReplyHandler    ( const TnmsAuthReply   & reply );


    /*  send message handling / packing  */

    bool                sendAuthRequest     ( TnmsAuthRequest & req );
    bool                sendAuthReply       ( TnmsAuthReply   & res );
    bool                sendMetric          ( TnmsMetric      & metric );
    bool                sendAdd             ( TnmsAdd         & add );
    bool                sendRemove          ( TnmsRemove      & rem );
    bool                sendRequest         ( TnmsRequest     & req );
    void                setLastRecord();

    /*  receive message handling / extraction  */

    int                 rcvAuthRequest      ( tnmsHeader  & hdr );
    int                 rcvAuthReply        ( tnmsHeader  & hdr );
    int                 rcvMetrics          ( tnmsHeader  & hdr );
    int                 rcvAdds             ( tnmsHeader  & hdr );
    int                 rcvRemoves          ( tnmsHeader  & hdr );
    int                 rcvRequests         ( tnmsHeader  & hdr );
    int                 rcvSubscribes       ( tnmsHeader  & hdr );
    int                 rcvUnsubscribes     ( tnmsHeader  & hdr );


private:

    void                init();
    void                clearState();
    void                closeConnection();
    void                setHostStr();

    bool                initHeader();



protected:

    TreeAuthFunctor *           _authorizations;
    AuthFunctor *               _authFunctor;
    SubscriptionList            _subscriptions;

    /* state */
    bool                        _connecting;
    bool                        _authenticating;
    bool                        _authorized;
    bool                        _subscribed;
    bool                        _subtree;

    bool                        _compression;

    std::string                 _hostname;
    std::string                 _login;
    std::string                 _hoststr;
    std::string                 _errstr;
    uint32_t                    _version;
    uint16_t                    _port;
    bool                        _debug;

private:

    BufferedSocket *            _sock;
    tnmsHeader *                _hdr;

    /*  Buffers */
    char *                      _rxbuff;
    size_t                      _rxbuffsz;

    char *                      _zxbuff;
    size_t                      _zxbuffsz;
    zip_ostream*                _zipper;
    std::ostringstream*         _zipout;

    CirBuffer*                  _wxcbuff;
    size_t                      _wxcbuffsz;
    char*                       _wptr;
    size_t                      _wtsize;
    size_t                      _wttotal;
    size_t                      _wxstall;
    int                         _loginAttempts;

    time_t                      _clTimeout;
    time_t                      _reconTime;
    time_t                      _lastWxTime;
    time_t                      _wxTimeout;

    bool                        _flush;
    uint32_t                    _flushLimit;
    uint64_t                    _maxMessages;

};


} // namespace

#endif  // _TNMSSOCKET_H_

