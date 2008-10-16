#ifndef _TNMSCORE_TNMSAUTHREPLY_H_
#define _TNMSCORE_TNMSAUTHREPLY_H_

#include "TnmsMessage.h"

namespace tnmsCore {

class TnmsAuthReply : public TnmsMessage {

public:

    TnmsAuthReply()
        : TnmsMessage(AUTH_REPLY)
    {}

    TnmsAuthReply ( const std::string & agent_name )
        : TnmsMessage(agent_name, AUTH_REPLY)
    {}

    TnmsAuthReply ( const std::string & agent_name, 
                    const TnmsOid     & oid )
        : TnmsMessage(agent_name, oid, AUTH_REPLY)
    {}

    eAuthTypes      authResult() const;
    std::string     authReason() const;
    std::string     authData()   const;

    virtual ssize_t serialize   ( char * buffer, size_t buffer_len );
    virtual ssize_t deserialize ( char * buffer, size_t buffer_len );
    virtual size_t  size() const;


protected:

    eAuthTypes     _auth_result;
    std::string    _auth_reason;
    std::string    _auth_data;
};


}  // namespace

#endif //  _TNMSCORE_TNMSAUTHREPLY_H_

