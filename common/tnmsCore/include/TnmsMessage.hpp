/*
 * TnmsMessage.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#ifndef _TNMSCORE_TNMSMESSAGE_H_
#define _TNMSCORE_TNMSMESSAGE_H_


namespace tnmscore {


#include "Serializable.hpp"


class TnmsMessage;

typedef TnmsMessage  TnmsRequestMessage;
typedef TnmsMessage  TnmsAddMessage;
typedef TnmsMessage  TnmsRemoveMessage;



class TnmsMessage : public Serializable {

public:
    TnmsMessage() {}

    TnmsMessage ( const std::string  & name )
        : _element_name(name)
    {}

    TnmsMessage ( const TnmsOid & oid )
        : _element_oid(oid)
    {}

    TnmsMessage ( const std::string & name, 
                  const TnmsOid     & oid )
        : _element_name(name),
          _element_oid(oid)
    {}

    virtual ~TnmsRequest() {}


    const std::string&  getElementName() const;
    const TnmsOid&      getElementOid()  const;

    // Serializable
    virtual ssize_t     serialize ( char * buffer, size_t buffer_len );
    virtual size_t      size() const;


protected:

    std::string  _element_name;
    TnmsOid      _element_oid;
};


}  // namespace

#endif /* _TNMSCORE_TNMSMESSAGE_H_ */
