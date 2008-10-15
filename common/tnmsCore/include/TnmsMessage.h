/*
 * TnmsMessage.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#ifndef _TNMSCORE_TNMSMESSAGE_H_
#define _TNMSCORE_TNMSMESSAGE_H_

#include <string>

#include "Serializable.hpp"
#include "tnmsProtocol.h"
#include "TnmsOid.h"

namespace tnmscore {

class TnmsMessage : public Serializable {

public:

    TnmsMessage ( int message_type = NO_RECORD );

    TnmsMessage ( const std::string  & name,
                  int message_type = RECORD_REQUEST );

    TnmsMessage ( const TnmsOid & oid,
                  int message_type = RECORD_REQUEST );

    TnmsMessage ( const std::string & name, 
                  const TnmsOid     & oid,
                  int message_type = RECORD_REQUEST );

    virtual ~TnmsMessage();

    const std::string&  getElementName() const;
    const TnmsOid&      getElementOid()  const;

    // Serializable
    virtual ssize_t     serialize   ( char * buffer, size_t buffer_len );
    virtual ssize_t     deserialize ( char * buffer, size_t buffer_len );
    virtual size_t      size() const;
    virtual uint16_t    message_type() const;


protected:

    std::string         _element_name;
    TnmsOid             _element_oid;
    uint16_t            _message_type;
};


typedef class TnmsMessage TnmsRequestMessage;


class TnmsAddMessage : public TnmsMessage {
public:
    TnmsAddMessage ( const std::string &  element_name,
                     const TnmsOid     &  oid )
        : TnmsMessage(element_name, oid, RECORD_ADD)
    {}

    virtual ~TnmsAddMessage() {}
};


class TnmsRemoveMessage : public TnmsMessage {
public:
    TnmsRemoveMessage ( const std::string  & element_name )
        : TnmsMessage(element_name, RECORD_REMOVE )
    {}
    TnmsRemoveMessage ( const TnmsOid      & oid )
        : TnmsMessage(oid, RECORD_REMOVE )
    {}
    TnmsRemoveMessage ( const std::string  & element_name,
                        const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, RECORD_REMOVE)
    {}

    virtual ~TnmsRemoveMessage() {}
};

class TnmsSubscribeMessage : public TnmsMessage {
public:
    TnmsSubscribeMessage ( const std::string  & element_name )
        : TnmsMessage(element_name, SUBSCRIBE )
    {}
    TnmsSubscribeMessage ( const TnmsOid      & oid )
        : TnmsMessage(oid, RECORD_REMOVE )
    {}
    TnmsSubscribeMessage ( const std::string  & element_name,
                        const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, SUBSCRIBE)
    {}

    virtual ~TnmsSubscribeMessage() {}
};

class TnmsUnsubscribeMessage : public TnmsMessage {
public:
    TnmsUnsubscribeMessage ( const std::string  & element_name )
        : TnmsMessage(element_name, UNSUBSCRIBE )
    {}
    TnmsUnsubscribeMessage ( const TnmsOid      & oid )
        : TnmsMessage(oid, RECORD_REMOVE )
    {}
    TnmsUnsubscribeMessage ( const std::string  & element_name,
                        const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, UNSUBSCRIBE)
    {}

    virtual ~TnmsUnsubscribeMessage() {}
};


}  // namespace

#endif /* _TNMSCORE_TNMSMESSAGE_H_ */
