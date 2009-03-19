/*
 * TnmsMetric.h
 *
 *  Copyright(c) 2008, Charlton Technology LLC
 *      Author: tcarland@gmail.com
 */

#ifndef _TNMSCORE_TNMSMETRIC_H_
#define _TNMSCORE_TNMSMETRIC_H_

#include "TnmsMessage.h"


namespace tnmsCore {


class TnmsMetric : public TnmsMessage {

  public:

    TnmsMetric();
    TnmsMetric ( const std::string & name, int message_type = METRIC_MESSAGE );
    TnmsMetric ( const std::string & name, const TnmsOid & oid );

    virtual ~TnmsMetric();


    int                 getValueType() const;


    const std::string&  getValue() const;
    bool                setValue    ( eValueType          valtype,
                                      const std::string & value );

    template<typename T>
    const T&            getValue() const
    {
        if ( _valType == TNMS_STRING ) 
            return 0;
        T  val = static_cast<T>(_value);
        return val;
    }

    template< typename T >
    bool                setValue    ( eValueType   valtype, 
                                      T &          value )
    {
        if ( valtype > TNMS_NONE && valtype < TNMS_STRING ) {
            _valType = valtype;
            _value   = static_cast<uint64_t>(value);
            return true;
        }
        return false;
    }


    const std::string&  getPvtData() const;
    bool                setPvtData  ( const std::string & data );

    /*  Serializable */

    virtual ssize_t     serialize   ( char * buffer, size_t buffer_len );
    virtual ssize_t     deserialize ( const char * buffer, size_t buffer_len );
    virtual size_t      size() const;


  protected:

    eValueType          _valType;
    uint64_t            _value;
    std::string         _valueStr;
    std::string         _pvt;

};

}  // namespace

#endif  /* _TNMSCORE_TNMSMETRIC_H_ */
