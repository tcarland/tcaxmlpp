#ifndef _INCLUDE_FWRULES_H_
#define _INCLUDE_FWRULES_H_


#include "fwgen.h"


namespace fwgen {

class FwVars;
class FwZones;


class FwRules {

public:

    typedef FwRuleList::iterator         iterator;
    typedef FwRuleList::const_iterator   const_iterator;
    
public:

    FwRules ( FwVars * vars = NULL, FwZones * zones = NULL,
              const std::string & protofile = FWGEN_PROTOCOLS_FILE);

    virtual ~FwRules();


    bool            parse          ( const std::string & rulefile );

    const_iterator  begin() const;
    const_iterator  end()   const;
    size_t          size()  const;

    std::string     getErrorStr() const;
    void            setDebug       ( bool d ) { _debug = d; }


protected: 

    bool            parseProto     ( const std::string & str, 
                                     uint16_t          & pval );

    bool            parseProtoFile ( const std::string & protofile );
    bool            resolveFwVar   ( const std::string & src,
                                     Prefix            & srcPrefix );
    bool            resolveFwPort  ( const std::string & portstr,
                                     FwPort            & fwport );

private:

    FwRuleList      _rules;

    FwProtoMap      _proto;

    FwVars        * _vars;
    FwZones       * _zones;
 
    std::string     _errstr;
    bool            _debug;


};


} // namespace

#endif /*_INCLUDE_FWRULES_H_*/
