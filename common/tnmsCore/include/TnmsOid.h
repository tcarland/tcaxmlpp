#ifndef _TNMSOID_H_
#define _TNMSOID_H_

#include <vector>
#include <string>

#include <inttypes.h>

namespace tnmsCore {


typedef std::vector<uint16_t>   OidList;
typedef uint16_t*               tOid;



class TnmsOid {

public:
    
    TnmsOid();
    TnmsOid ( const OidList & oidlist );
    TnmsOid ( const std::string & oidstr );
    TnmsOid ( TnmsOid & oid, size_t oidlen );
    TnmsOid ( struct tnmsOid & toid );
    TnmsOid ( const  TnmsOid & toid );
    
    virtual ~TnmsOid();
    
    uint16_t        operator[] ( uint32_t indx );
    
    std::string     toString() const;
    tOid            toOidArray() const;
    
    const OidList&  getOidList() const;
    OidList&        getOidLIst();
    
    uint32_t        getOidLength() const;
    uint32_t        size() const { return this->getOidLength(); }
    
    uint16_t        lastValue();
    
    size_t          serialize ( char * buffer, size_t len );
    
    
private:
    
    OidList *       _oid;
    
};

}  // namespace

#endif // _TNMSOID_H_

