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

    typedef OidList::iterator        iterator;
    typedef OidList::const_iterator  const_iterator;

public:
    
    TnmsOid();
    TnmsOid ( const OidList & oidlist );
    TnmsOid ( const std::string & oidstr );
    TnmsOid ( const TnmsOid & toid );
    
    virtual ~TnmsOid();
    
    uint16_t        operator[] ( uint32_t indx );
    
    std::string     toString() const;
    tOid            toArray() const;
    
    const OidList&  getOidList() const;
    OidList&        getOidList();

    iterator        begin();
    iterator        end();
    const_iterator  begin() const;
    const_iterator  end() const;
    bool            empty() const;
    
    const uint16_t& lastValue() const;
    uint32_t        getOidLength() const;
    uint32_t        size() const { return this->getOidLength(); }
    
    size_t          serialize ( char * buffer, size_t len );
    

    static void     StringToOidList ( const std::string & oidstr,
                                      OidList & oidlist );

private:
    
    OidList *       _oidlist;
    
};

}  // namespace

#endif // _TNMSOID_H_

