/*
 * TnmsMessage.h
 *
 *  Copyright(c) 2008, Timothy Charlton Arland
 *  tcarland@gmail.com
 */
#ifndef _TNMSCORE_TNMSMESSAGE_H_
#define _TNMSCORE_TNMSMESSAGE_H_

#include <string>

#include "tnmsProtocol.h"
#include "TnmsOid.h"

#include "Serializable.hpp"

namespace tnmsCore {



/**  TnmsMessage is the base message type of all system messages. The 
  *  default TnmsMessage object is of the 'Request' type. Other messages
  *  are simply a TnmsMessage object with a different message_type set, 
  *  such as Add and Remove. The 'TnmsMetric' message, is the only 
  *  specialized version of a TnmsMessage object.
 **/
class TnmsMessage : public tcanetpp::Serializable {

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

    
    virtual void        operator=     ( const TnmsMessage & msg );
    virtual bool        operator==    ( const TnmsMessage & msg ) const;
    virtual bool        operator<     ( const TnmsMessage & msg ) const;

    const std::string&  getElementName() const { return this->_element_name; }
    const TnmsOid&      getElementOid()  const { return this->_element_oid; }
    const eMessageType& getMessageType() const { return this->_message_type; }

    virtual uint16_t    message_type()   const;
    virtual void        message_type  ( eMessageType  msgtype );

    // Serializable
    virtual ssize_t     serialize     ( char * buffer, size_t buffer_len );
    virtual ssize_t     deserialize   ( const char * buffer, size_t buffer_len );
    virtual size_t      size() const;


protected:

    std::string         _element_name;
    TnmsOid             _element_oid;
    eMessageType        _message_type;
};



typedef class TnmsMessage TnmsRequest;



/**  The TnmsAdd class is simply a conveniece class that inherits 
  *  the TnmsMessage class and sets the message_type accordingly.
 **/
class TnmsAdd : public TnmsMessage {

  public:

    TnmsAdd() : TnmsMessage(ADD_MESSAGE) {}

    TnmsAdd ( const std::string &  element_name )
        : TnmsMessage(element_name, ADD_MESSAGE) {}
    TnmsAdd ( const std::string &  element_name,
              const TnmsOid     &  oid )
        : TnmsMessage(element_name, oid, ADD_MESSAGE) {}

    virtual ~TnmsAdd() {}
};



/**  The TnmsRemove class is simply a conveniece class that 
  *  inherits the TnmsMessage class and sets the message_type 
  *  accordingly.
 **/
class TnmsRemove : public TnmsMessage {

  public:

    TnmsRemove() : TnmsMessage(REMOVE_MESSAGE) {}

    TnmsRemove ( const std::string  & element_name )
        : TnmsMessage(element_name, REMOVE_MESSAGE ) {}

    TnmsRemove ( const TnmsOid      & oid )
        : TnmsMessage(oid, REMOVE_MESSAGE ) {}

    TnmsRemove ( const std::string  & element_name,
                 const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, REMOVE_MESSAGE) {}

    virtual ~TnmsRemove() {}
};



/**  The TnmsSubscribe class is simply a conveniece class that 
  *  inherits the TnmsMessage class and sets the message_type 
  *  accordingly.
 **/
class TnmsSubscribe : public TnmsMessage {

  public:

    TnmsSubscribe() : TnmsMessage(SUBSCRIBE) {}

    TnmsSubscribe ( const std::string  & element_name )
        : TnmsMessage(element_name, SUBSCRIBE ) {}

    TnmsSubscribe ( const TnmsOid      & oid )
        : TnmsMessage(oid, SUBSCRIBE ) {}

    TnmsSubscribe ( const std::string  & element_name,
                    const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, SUBSCRIBE) {}

    virtual ~TnmsSubscribe() {}
};



/**  The TnmsUnsubscribe class is simply a conveniece class that 
  *  inherits the TnmsMessage class and sets the message_type 
  *  accordingly.
 **/
class TnmsUnsubscribe : public TnmsMessage {

  public:

    TnmsUnsubscribe()
        : TnmsMessage(UNSUBSCRIBE) {}

    TnmsUnsubscribe ( const std::string  & element_name )
        : TnmsMessage(element_name, UNSUBSCRIBE ) {}

    TnmsUnsubscribe ( const TnmsOid      & oid )
        : TnmsMessage(oid, UNSUBSCRIBE ) {}

    TnmsUnsubscribe ( const std::string  & element_name,
                      const TnmsOid      & oid )
        : TnmsMessage(element_name, oid, UNSUBSCRIBE) {}

    virtual ~TnmsUnsubscribe() {}
};


}  // namespace

#endif /* _TNMSCORE_TNMSMESSAGE_H_ */
