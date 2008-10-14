#ifndef _TNMSCORE_TNMSREQUESTMESSAGE_H_
#define _TNMSCORE_TNMSREQUESTMESSAGE_H_

#include "TnmsMessage.h"


namespace tnmscore {


class TnmsRequestMessage : public TnmsMessage {

public:

    TnmsRequestMessage();
    TnmsRequestMessage ( const std::string & name );
    TnmsRequestMessage ( const TnmsOid     & oid );
    TnmsRequestMessage ( const std::string & name,
                         const TnmsOid     & oid );

    virtual ~TnmsRequestMessage() {}

    virtual ssize_t  serializable ( char * buffer, size_t buffer_len );
    virtual size_t   size() const;
};


class TnmsAddMessage : public TnmsRequestMessage {

public:

    TnmsAddMessage ( const std::string &  element_name,
                     const TnmsOid     &  oid )
        : TnmsMessage(element_name, oid, RECORD_ADD)
    {}

    virtual ~TnmsAddMessage() {}
};


class TnmsRemoveMessage : public TnmsRequestMessage {

public:

    TnmsRemoveMessage ( const std::string  & element_name )
        : TnmsRequestMessage(element_name, RECORD_REMOVE )
    {}
    TnmsRemoveMessage ( const TnmsOid      & oid )
        : TnmsRequestMessage(oid, RECORD_REMOVE )
    {}
    TnmsRemoveMessage ( const std::string  & element_name,
                        const TnmsOid      & oid )
        : TnmsRequestMessage(element_name, oid, RECORD_REMOVE)
    {}

    virtual ~TnmsRemoveMessage() {}
};


} // namespace

#endif  //  _TNMSCORE_TNMSREQUESTMESSAGE_H_

