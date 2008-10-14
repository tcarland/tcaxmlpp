/*
 * TnmsMetricMessage.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */

#ifndef _TNMSCORE_TNMSMETRICMESSAGE_H_
#define _TNMSCORE_TNMSMETRICMESSAGE_H_

#include "TnmsOid.h"
#include "tnmsProtocol.h"
#include "TnmsMessage.hpp"


namespace tnmscore {


class TnmsMetricMessage : public TnmsMessage {

public:

    TnmsMetricMessage();

    TnmsMetricMessage ( const std::string & name,
                        const TnmsOid & oid );

    virtual ~TnmsMetricMessage();


    std::string  getValue();
    template<typename T>
    T            getValue();

    int          getValueType()  const;

    bool         setValue  ( eValueType    valtype, 
                             std::string & value );

    template< typename T >
    bool         setValue  ( eValueType valtype, T & value );

    //  Serializable
    ssize_t      serialize ( char * buffer, size_t sz );
    size_t       size() const;
    //uint16_t     message_type() const;


protected:

    eValueType          _valType;
    uint64_t            _value;
    std::string         _valueStr;
    std::string         _pvtData;

};

}  // namespace

#endif  /* _TNMSCORE_TNMSMETRICMESSAGE_H_ */
