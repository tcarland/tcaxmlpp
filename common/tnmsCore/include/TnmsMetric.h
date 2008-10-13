/*
 * TnmsMetric.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */

#ifndef _TNMSCORE_TNMSMETRIC_H_
#define _TNMSCORE_TNMSMETRIC_H_

#include "TnmsOid.h"
#include "tnmsProtocol.h"
#include "Serializable.hpp"


namespace tnmscore {


class TnmsMetric : public Serializable {

public:

    TnmsMetric();
    TnmsMetric ( const std::string & name, TnmsOid * oid );

    virtual ~TnmsMetric();


    const std::string&  getName() const;
    const TnmsOid&      getOid( ) const;

    size_t              size()    const;
    inline size_t       getSize() const { return this->size(); }

    /*  Overloaded getValue() for retrieving a string or a number.
     *  Due to string support being somewhat dynamic, the type 
     *  should be checked via getValueType() 
     */
    std::string  getValue();
    template<typename T>
    T&           getValue();
    int          getValueType()  const;


    bool         setValue  ( eValueType  valtype, std::string & value );
    template< typename T >
    bool         setValue  ( eValueType  valtype, T  & value );


    ssize_t      serialize ( char  * buffer, size_t  sz );


private:

    std::string         _name;
    TnmsOid *           _oid;

    eValueType          _valType;
    uint64_t            _value;
    std::string         _valueStr;

};

}  // namespace

#endif  /* _TNMSCORE_TNMSMETRIC_H_ */
