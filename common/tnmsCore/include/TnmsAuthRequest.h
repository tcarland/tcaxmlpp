#ifndef _TNMSCORE_TNMSAUTHREQUEST_H_
#define _TNMSCORE_TNMSAUTHREQUEST_H_

#include "TnmsMessage.h"

namespace tnmsCore {


class TnmsAuthRequest : public TnmsMessage {

public:

    TnmsAuthRequest()
        : TnmsMessage(AUTH_REQUEST)
    {}

    TnmsAuthRequest ( const std::string & agent_name,
                      const std::string & agent_key )
        : TnmsMessage(agent_name, AUTH_REQUEST),
          _agent_key(agent_key)
    {}

    virtual ~TnmsAuthRequest() {}

    ipv4addr_t       ipaddr() const { return _ipaddr; }
    void             ipaddr      ( ipv4addr_t addr ) { _ipaddr = addr; }

    std::string      agent_key() const { return _agent_key; }

    // serializable
    virtual ssize_t  serialize   ( char * buffer, size_t buffer_len ) const;
    virtual ssize_t  deserialize ( const char * buffer, size_t buffer_len );
    virtual size_t   size() const;

protected:

    std::string     _agent_key;
    ipv4addr_t      _ipaddr;
};

       
} // namespace

#endif  // _TNMSCORE_TNMSAUTHREQUEST_H_
