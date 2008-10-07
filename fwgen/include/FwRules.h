#ifndef _INCLUDE_FWRULES_H_
#define _INCLUDE_FWRULES_H_

#include "fwgen.h"


namespace fwgen {


struct FwPort {
    uint16_t  port;
    uint16_t  port_high;
    bool      ranged;
};

enum FwProtocol {
    FWPROTO_NONE,
    FWPROTO_TCP = 17,
    FWPROTO_UDP = 9,
};


struct FwRule {
    bool      permit;
    Prefix    src;
    FwPort    srcport;
    Prefix    dst;
    FwPort    dstport;
    uint16_t  proto;
    bool      established;
};

typedef std::set<FwRule*> FwRuleSet;


class FwRules {
	
public:
	
	FwRules();
	FwRules ( const std::string & rulefile );
	
	~FwRules();
	
	
	bool	     parse	( const std::string & rulefile );
	
	
	std::string  getErrorStr() const;
	
	
private:
	
	
	std::string   & errStr;
	
	
};


}

#endif /*_INCLUDE_FWRULES_H_*/
