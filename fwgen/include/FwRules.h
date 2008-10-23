#ifndef _FWGEN_FWRULES_H_
#define _FWGEN_FWRULES_H_


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


    bool            parse           ( const std::string & rulefile );

    const_iterator  begin() const;
    const_iterator  end()   const;
    size_t          size()  const;

    std::string     getErrorStr() const;
    void            setDebug        ( bool d ) { _debug = d; }


protected: 

    bool            resolveProtocol ( const std::string & str, 
                                      uint16_t          & pval );

    bool            resolveFwVar    ( const std::string & src,
                                      Prefix            & srcPrefix );
    bool            resolveFwPort   ( const std::string & portstr,
                                      FwPort            & fwport );

    bool            parseProtoFile  ( const std::string & protofile );


private:

    FwVars *               _vars;
    FwZones *              _zones;
 
    FwRuleList             _rules;
    FwProtoMap             _proto;

    std::string            _errstr;
    bool                   _debug;

};


} // namespace

#endif  /* _FWGEN_FWRULES_H_ */

