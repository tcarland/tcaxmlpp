/*
 * TnmsMessage.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#ifndef _TNMSCORE_TNMSMESSAGE_H_
#define _TNMSCORE_TNMSMESSAGE_H_

#include <string>

#include "tnmsProtocol.h"
#include "Serializable.hpp"
#include "TnmsOid.h"

namespace tnmsCore {


class TnmsMessage : public Serializable {

public:

    TnmsMessage ( eMessageType         message_type = NO_RECORD );

    TnmsMessage ( const std::string  & name,
                  eMessageType         message_type = REQUEST_MESSAGE );

    TnmsMessage ( const TnmsOid      & oid,
                  eMessageType         message_type = REQUEST_MESSAGE );

    TnmsMessage ( const std::string  & name,
                  const TnmsOid      & oid,
                  eMessageType         message_type = REQUEST_MESSAGE );

    virtual ~TnmsMessage();

    const std::string&  getElementName() const;
    const TnmsOid&      getElementOid()  const;

    // Serializable
    virtual ssize_t     serialize   ( char * buffer, size_t buffer_len );
    virtual ssize_t     deserialize ( const char * buffer, size_t buffer_len );
    virtual size_t      size() const;
    virtual uint16_t    message_type() const;
    virtual void        message_type ( eMessageType  msgtype );


protected:

    std::string         _element_name;
    TnmsOid             _element_oid;
    eMessageType        _message_type;
};


typedef class TnmsMessage TnmsRequest;


class TnmsAdd : public TnmsMessage {

  public:

    TnmsAdd() : TnmsMessage(ADD_MESSAGE) {}

    TnmsAdd ( const std::string &  element_name )
        : TnmsMessage(element_name, ADD_MESSAGE)
    {}
    TnmsAdd ( const std::string &  element_name,
              const TnmsOid     &  oid )
        : TnmsMessage(element_name, oid, ADD_MESSAGE)
    {}

    virtual ~TnmsAdd() {}
};


class TnmsRemove : public TnmsMessage {

  public:

    TnmsRemove() : TnmsMessage(REMOVE_MESSAGE) {}

    TnmsRemove ( const std::string  & element_name )
        : TnmsMessage(element_name, REMOVE_MESSAGE )
    {}

    TnmsRemove ( const TnmsOid      & oid )
        : TnmsMessage(oid, REMOVE_MESSAGE )
    {}

    TnmsRemove ( const std::string  & element_name,
                 const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, REMOVE_MESSAGE)
    {}

    virtual ~TnmsRemove() {}
};


class TnmsSubscribe : public TnmsMessage {

  public:

    TnmsSubscribe() : TnmsMessage(SUBSCRIBE) {}

    TnmsSubscribe ( const std::string  & element_name )
        : TnmsMessage(element_name, SUBSCRIBE )
    {}

    TnmsSubscribe ( const TnmsOid      & oid )
        : TnmsMessage(oid, SUBSCRIBE )
    {}

    TnmsSubscribe ( const std::string  & element_name,
                    const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, SUBSCRIBE)
    {}

    virtual ~TnmsSubscribe() {}
};



class TnmsUnsubscribe : public TnmsMessage {

  public:

    TnmsUnsubscribe()
        : TnmsMessage(UNSUBSCRIBE)
    {}

    TnmsUnsubscribe ( const std::string  & element_name )
        : TnmsMessage(element_name, UNSUBSCRIBE )
    {}

    TnmsUnsubscribe ( const TnmsOid      & oid )
        : TnmsMessage(oid, UNSUBSCRIBE )
    {}

    TnmsUnsubscribe ( const std::string  & element_name,
                      const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, UNSUBSCRIBE)
    {}

    virtual ~TnmsUnsubscribe() {}
};


}  // namespace

#endif /* _TNMSCORE_TNMSMESSAGE_H_ */
