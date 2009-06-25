#ifndef _TNMSCORE_TNMSAUTHREPLY_H_
#define _TNMSCORE_TNMSAUTHREPLY_H_

#include "TnmsMessage.h"

namespace tnmsCore {


class TnmsAuthReply : public TnmsMessage {

public:

    TnmsAuthReply()
        : TnmsMessage(AUTH_REPLY),
          _auth_result(AUTH_NO_RESULT)
    {}

    TnmsAuthReply ( const std::string & agent_name )
        : TnmsMessage(agent_name, AUTH_REPLY),
          _auth_result(AUTH_NO_RESULT)
    {}

    TnmsAuthReply ( const std::string & agent_name, 
                    const TnmsOid     & oid )
        : TnmsMessage(agent_name, oid, AUTH_REPLY),
          _auth_result(AUTH_NO_RESULT)
    {}

    eAuthType           authResult() const;
    const std::string&  authReason() const;
    const std::string&  authData()   const;

    void                authResult  ( eAuthType result ) { this->_auth_result = result; }
    void                authReason  ( const std::string & reason ) { _auth_reason = reason; }
    void                authData    ( const std::string & data ) { _auth_data = data; }

    // serializable
    virtual ssize_t     serialize   ( char * buffer, size_t buffer_len ) const;
    virtual ssize_t     deserialize ( const char * buffer, size_t buffer_len );
    virtual size_t      size() const;


protected:

    eAuthType      _auth_result;
    std::string    _auth_reason;
    std::string    _auth_data;
};


}  // namespace

#endif //  _TNMSCORE_TNMSAUTHREPLY_H_

