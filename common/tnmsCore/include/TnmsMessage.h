/*
 * TnmsMessage.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#ifndef _TNMSCORE_TNMSMESSAGE_H_
#define _TNMSCORE_TNMSMESSAGE_H_

#include "Serializable.hpp"
#include "tnmsProtocol.h"


namespace tnmscore {


class TnmsMessage : public Serializable {

public:

    TnmsMessage ( int message_type = RECORD_NONE );

    TnmsMessage ( const std::string  & name,
                  int message_type = RECORD_REQUEST );

    TnmsMessage ( const TnmsOid & oid,
                  int message_type = RECORD_REQUEST );

    TnmsMessage ( const std::string & name, 
                  const TnmsOid     & oid,
                  int message_type = RECORD_REQUEST );

    virtual ~TnmsRequest();


    const std::string&  getElementName() const;
    const TnmsOid&      getElementOid()  const;


    // Serializable
    virtual ssize_t     serialize ( char * buffer, size_t buffer_len );
    virtual size_t      size() const;
    virtual uint16_t    message_type() const;


protected:

    std::string         _element_name;
    TnmsOid             _element_oid;
    uint16_t            _message_type;

};


}  // namespace

#endif /* _TNMSCORE_TNMSMESSAGE_H_ */
