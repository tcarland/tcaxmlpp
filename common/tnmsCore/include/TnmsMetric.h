/*
 * TnmsMetric.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */

#ifndef _TNMSCORE_TNMSMETRIC_H_
#define _TNMSCORE_TNMSMETRIC_H_

#include "TnmsMessage.h"



namespace tnmsCore {


class TnmsMetric : public TnmsMessage {

public:

    TnmsMetric();
    TnmsMetric ( const std::string & name, const TnmsOid & oid );

    virtual ~TnmsMetric();


    int                 getValueType() const;


    const std::string&  getValue() const;
    template<typename T>
    const T&            getValue() const;

    bool                setValue    ( eValueTypes valtype,
                                      const std::string & value );
    template< typename T >
    bool                setValue    ( eValueTypes valtype, T & value );


    const std::string&  getPvtData() const;
    bool                setPvtData ( const std::string &  data );

    /*  Serializable */

    virtual ssize_t serialize   ( char * buffer, size_t buffer_len );
    virtual ssize_t deserialize ( char * buffer, size_t buffer_len );
    virtual size_t  size() const;


protected:

    eValueTypes         _valType;
    uint64_t            _value;
    std::string         _valueStr;
    std::string         _pvt;

};

}  // namespace

#endif  /* _TNMSCORE_TNMSMETRIC_H_ */
