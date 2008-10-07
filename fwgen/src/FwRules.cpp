#define _SOURCE_FWRULES_CPP_

#include <vector>

#include "FwRules.h"

#include "Exception.h"
using namespace tca::net;


namespace fwgen {

FwRules::FwRules() {}

FwRules::FwRules ( const std::string & rulefile )
{
	if ( ! this->parse(rulefile) )
		throw Exception(this->getErrorStr());
}


bool
FwRules::parse ( const std::string & rulefile )
{
    std::string               ln;
    std::string::size_type    indx;
    std::vector<std::string>  rulev;

    std::vector<std::string>::iterator  vIter;

    std::ifstream ifn(rulefile.c_str());

    if ( ! ifn )
        return false;

    while ( std::getline(ifn, ln) ) {
        StringUtils::trim(ln);
 
        // ignoring comment
        if ( StringUtils::startsWith(ln, "#") || StringUtils::startsWith(ln, ";") )
            continue;

        indx = ln.find_first_of('#');

        if ( indx != std::string::npos )
            ln = ln.substr(0, indx);

        StringUtils::split(ln, ' ', std::back_inserter(rulev));

        if ( rulev.size() < 5 ) {
            _errstr = "Rule parse error with line: " + ln;
            return false;
        }

        FwRule * fwrule = new FwRule();

        if ( rulev[0].compare("permit") == 0 )
            fwrule->permit = true;
        else if ( rulev[0].compare("deny") == 0 )
            fwrule->permit = false;
        else {
            _errstr = "Rule parse error with line: " + ln;
            return false;
        }
      
        

        //for ( vIter = rulev.begin(); vIter != rulev.end(); ++vIter )
            //std::cout << *vIter << " ";
        //std::cout << std::endl;

        rulev.clear();
    }
	return true;
}


std::string
FwRules::getErrorStr() const 
{
	return this->_errStr;
}


} // namespace

// _SOURCE_FWRULES_CPP_
