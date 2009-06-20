/*
 * TnmsSocket.h
 *
 *  Copyright(c) 2008, Timothy Charlton Arland
 *      Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_TNMSSOCKET_H_
#define _TNMSCORE_TNMSSOCKET_H_

#include <sstream>
#include <set>


#include "BufferedSocket.h"
#include "CircularBuffer.h"
using namespace tcanetpp;


#include "tnmsCore.h"

#include "TreeAuthorizations.hpp"
#include "MessageHandler.hpp"
#include "TnmsMetric.h"
#include "TnmsAuthRequest.h"
#include "TnmsAuthReply.h"


#include "zipstream.hpp"
using namespace zlib_stream;


namespace tnmsCore {


typedef std::set<std::string>  SubscriptionList;


class TnmsSocket {

  public:

    TnmsSocket  ( MessageHandler * msgHandler = NULL );

    TnmsSocket  ( const std::string & host, uint16_t port,
                  MessageHandler * msgHandler = NULL );

    TnmsSocket  ( tcanetpp::BufferedSocket * sock,
                  MessageHandler * msgHandler = NULL );

    virtual ~TnmsSocket();


    int                 openConnection  ( const std::string & host,
                                          uint16_t port );
    int                 openConnection();


    virtual int         connect()      { return this->openConnection(); }
    virtual void        close()        { return this->closeConnection(); }

    virtual int         send           ( const time_t  & now );
    virtual int         receive        ( const time_t  & now );

    ssize_t             flush();
    size_t              txBytesBuffered();

    void                setLastRecord();

    size_t              getBytesSent();
    size_t              getBytesReceived();

    /*  Attributes  */

    bool                isConnected();
    bool                isConnecting();

    sockfd_t            getDescriptor() const;
    sockfd_t            getSockFD() const  { return this->getDescriptor(); }
    sockfd_t            getFD() const      { return this->getDescriptor(); }

    void                setMessageHandler  ( MessageHandler * msgHandler );
    MessageHandler*     getMessageHandler();

    void                reconnectTime      ( const time_t & secs );
    const time_t&       reconnectTime() const;

    void                setCompression     ( bool    compress );
    bool                getCompression() const;

    void                setTxQueueSize     ( size_t  bytes );
    size_t              getTxQueueSize() const;

    std::string         getClientLogin() const;

    std::string         getHostStr()  const;
    std::string         getAddrStr()  const;
    std::string         getHostname() const;
    uint16_t            getHostPort() const;
    uint32_t            getVersion()  const;

    std::string         getErrorStr() const;
    void                setDebug          ( bool debug );


    /*  Additional attributes */

    void                enableFlush       ( bool  flush );
    bool                flushEnabled();
    void                setFlushLimit     ( uint32_t limit );
    uint32_t            getFlushLimit();

    void                enableTimeout     ( bool   timeout );
    void                setTimeout        ( const time_t & secs );
    const time_t&       getTimeout() const;


    /*  Authorization / Subscriptions  */

    virtual void        login             ( const std::string &  user,
                                            const std::string &  pw );

    virtual void        authReply         ( const TnmsAuthReply & reply );

    bool                isAuthorized() const;
    bool                isSubscribed() const;

    virtual bool        subscribeStructure();
    virtual bool        unsubscribeStructure();

    virtual bool        subscribeAll();
    virtual bool        unsubscribeAll();

    virtual bool        subscribe         ( const std::string & name );
    virtual bool        subscribe         ( const TnmsOid     & oid );
    virtual bool        unsubscribe       ( const std::string & oid );
    virtual bool        unsubscribe       ( const TnmsOid     & oid );

    virtual bool        subscribeLevel    ( const std::string & oid );
    virtual bool        unsubscribeLevel  ( const std::string & oid );

    int                 subscribeCount();
    void                resubscribe();

    /*  send message handling / packing  */

    bool                sendMessage       ( Serializable  * message );

  protected:

    /*  receive message handling / extraction  */
    int                 receiveMessages   ( tnmsHeader  & hdr );

    int                 rcvAuthRequest    ( tnmsHeader  & hdr );
    int                 rcvAuthReply      ( tnmsHeader  & hdr );
    int                 rcvMetrics        ( tnmsHeader  & hdr );
    int                 rcvAdds           ( tnmsHeader  & hdr );
    int                 rcvRemoves        ( tnmsHeader  & hdr );
    int                 rcvRequests       ( tnmsHeader  & hdr );
    int                 rcvSubscribes     ( tnmsHeader  & hdr );
    int                 rcvUnsubscribes   ( tnmsHeader  & hdr );


  private:

    void                init();
    void                clearState();
    void                closeConnection();
    void                setHostStr();

    bool                initHeader        ( uint16_t type, size_t size );
    ssize_t             uncompress        ( uint32_t size );
    //bool                checkStall();


  protected:

    TreeAuthFunctor *           _authFunctor;
    TreeAuthList                _authorizations;
    MessageHandler *            _msgHandler;
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
    std::string                 _authname;
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

    size_t                      _rxCtr;
    size_t                      _txCtr;
    int                         _loginCtr;

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

