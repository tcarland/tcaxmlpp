/*
 * TnmsMetric.h
 *
 *  Copyright(c) 2008,2009  Timothy Charlton Arland
 *  tcarland@gmail.com
 */
#ifndef _TNMSCORE_TNMSMETRIC_H_
#define _TNMSCORE_TNMSMETRIC_H_

#include "TnmsMessage.h"


namespace tnmsCore {


class TnmsMetric : public TnmsMessage {

  public:

    TnmsMetric();
    TnmsMetric ( const std::string & name );
    TnmsMetric ( const std::string & name, const TnmsOid & oid );

    virtual ~TnmsMetric();


    virtual void        operator=   ( const TnmsMetric & metric );
    virtual TnmsMetric& operator+=  ( const TnmsMetric & metric );


    virtual void        reset();



    eValueType          getValueType() const;


    const std::string&  getValue() const;


    template<typename T>
    T                   getValue() const
    {
        if ( _valType == TNMS_STRING ) 
            return 0;
        T  val = static_cast<T>(_value);
        return val;
    }


    bool                setValue    ( eValueType          valtype,
                                      const std::string & value );


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

    template<typename T>
    T                   getValueAvg() const
    {
        if ( _valType == TNMS_STRING ) 
            return 0;
        T  val = static_cast<T>(_valueAvg);
        return val;
    }


    template< typename T >
    bool                setValueAvg ( eValueType   valtype, 
                                      T &          value )
    {
        if ( (valtype > TNMS_NONE && valtype < TNMS_STRING) 
              && valtype == _valType ) 
        {
            _valueAvg = static_cast<uint64_t>(value);
            return true;
        }
        return false;
    }

    uint32_t            getTimestamp() const;
    void                setTimestamp ( uint32_t epoch );

    const std::string&  getPvtData() const;
    bool                setPvtData  ( const std::string & data );

    uint32_t            getSamples() const;
    void                setSamples ( uint32_t samples );

    /*  Serializable */
    virtual ssize_t     serialize   ( char * buffer, size_t buffer_len ) const;
    virtual ssize_t     deserialize ( const char * buffer, size_t buffer_len );
    virtual size_t      size() const;


  protected:

    eValueType          _valType;
    uint64_t            _value;
    uint64_t            _valueAvg;
    uint64_t            _valueT;
    uint32_t            _samples;
    uint32_t            _timestamp;
    std::string         _valueStr;
    std::string         _pvt;

};

}  // namespace


#endif  // _TNMSCORE_TNMSMETRIC_H_

