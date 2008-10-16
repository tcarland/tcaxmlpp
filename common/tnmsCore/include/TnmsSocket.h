#ifndef _TNMSCORE_TNMSSOCKET_H_
#define _TNMSCORE_TNMSSOCKET_H_

#include <sstream>
#include <set>

#include "BufferedSocket.h"
#include "CircularBuffer.h"
using namespace tcanetpp;

#include "tnmsCore.h"
#include "TreeAuthorizations.hpp"
#include "TnmsMessage.h"
#include "TnmsMetric.h"
#include "TnmsAuthRequest.h"
#include "TnmsAuthReply.h"
#include "TnmsOid.h"

#include "zipstream.hpp"
using namespace zlib_stream;



namespace tnmsCore {


typedef std::set<std::string>  SubscriptionList;

class TnmsSocket {

public:

    TnmsSocket  ( uint32_t  flush_limit = TNMS_FLUSH_LIMIT );

    TnmsSocket  ( ipv4addr_t ip, uint16_t port,
                  uint32_t  flush_limit = TNMS_FLUSH_LIMIT );

    TnmsSocket  ( tcanetpp::BufferedSocket * sock,
                  uint32_t  flush_limit = TNMS_FLUSH_LIMIT );

    virtual ~TnmsSocket();


    int                 openConnection  ( const std::string & host,
                                          uint16_t port );
    int                 openConnection();


    virtual int         connect()  { return this->openConnection(); }
    virtual void        close()    { return this->closeConnection(); }

    int                 send();
    int                 send       ( const time_t  & now );
    int                 receive();
    int                 receive    ( const time_t  & now );

    ssize_t             flush();
    size_t              txBytesBuffered();


    /*  Attributes  */

    bool                isConnected();
    bool                isConnecting();

    sockfd_t            getDescriptor() const;
    sockfd_t            getSockFD() const    { return this->getDescriptor(); }

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
    void                setTimeout          ( const time_t & secs );
    const time_t&       getTimeout() const;


    /*  Subscriptions  */

    void                login                ( const std::string &  user,
                                               const std::string &  pw );

    bool                isAuthorized() const;
    bool                isSubscribed() const;

    virtual bool        subscribeStructure();
    virtual bool        unsubscribeStructure();

    virtual bool        subscribeAll();
    virtual bool        unsubscribeAll();

    virtual bool        subscribe           ( const std::string & name );
    virtual bool        subscribe           ( const TnmsOid     & oid );
    virtual bool        unsubscribe         ( const std::string & oid );
    virtual bool        unsubscribe         ( const TnmsOid     & oid );

    virtual bool        subscribeLevel      ( const std::string & oid );
    virtual bool        unsubscribeLevel    ( const std::string & oid );

    int                 subscribeCount();
    void                resubscribe();


    /*  Messaging Callbacks  */

    virtual void        AddHandler          ( const TnmsAdd     & add ) {}
    virtual void        RemoveHandler       ( const TnmsRemove  & remove ) {}
    virtual void        MetricHandler       ( const TnmsMetric  & metric ) {}
    virtual void        RequestHandler      ( const TnmsRequest & request ) {}

    virtual void        SubscribeHandler    ( const std::string & name ) {}
    virtual void        UnsubscribeHandler  ( const std::string & name ) {}
    virtual void        StructureHandler    ( bool  subscribe ) {}

    virtual void        LastRecordHandler   ( int   record_type ) {}

    virtual void        AuthRequestHandler  ( const TnmsAuthRequest & request ) {}
    virtual void        AuthReplyHandler    ( const TnmsAuthReply   & reply );


    /*  send message handling / packing  */

    virtual bool        sendMessage         ( Serializable  * message );


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

    bool                initHeader          ( uint16_t type, size_t size );
    ssize_t             uncompress          ( uint32_t size );
    //bool                checkStall();
    //void                clearStall() {}



protected:

    TreeAuthFunctor *           _authFunctor;
    TreeAuthList                _authorizations;
    SubscriptionList            _subs;

    /* state */
    bool                        _connecting;
    bool                        _authorizing;
    bool                        _authorized;
    bool                        _subscribed;
    bool                        _subtree;

    bool                        _compression;

    std::string                 _hostname;
    std::string                 _login;
    std::string                 _hoststr;
    std::string                 _addrstr;
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

    CircularBuffer*             _wxcbuff;
    size_t                      _wxcbuffsz;
    char*                       _wptr;
    size_t                      _wtsize;
    size_t                      _wtt;
    size_t                      _wxstallsz;
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

#endif  // _TNMSCORE_TNMSSOCKET_H_

