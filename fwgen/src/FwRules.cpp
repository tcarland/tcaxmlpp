#define _FWGEN_FWRULES_CPP_

#include <fstream>
#include <iostream>
#include <vector>

#include "FwRules.h"
#include "FwVars.h"
#include "FwZones.h"

#include "Exception.hpp"
#include "StringUtils.h"
#include "CidrUtils.h"
using namespace tcanetpp;


namespace fwgen {

/*
FwRules::FwRules() 
    : _vars(NULL),
      _zones(NULL)
{}
*/

FwRules::FwRules ( FwVars * vars, FwZones * zones, 
                   const std::string & protofile )
    : _vars(vars),
      _zones(zones)
{
    this->parseProtoFile(protofile);
}


FwRules::~FwRules() {}


bool
FwRules::parse ( const std::string & rulefile )
{
    std::string               ln;
    //std::string::size_type    indx;
    std::vector<std::string>  rulev;

    std::vector<std::string>::iterator  vIter;

    std::ifstream  ifn(rulefile.c_str());

    if ( ! ifn )
        return false;

    while ( std::getline(ifn, ln) ) {
        StringUtils::trim(ln);
        StringUtils::stripComments(ln);
        StringUtils::replaceTabs(ln);

        StringUtils::split(ln, ' ', std::back_inserter(rulev));

        if ( rulev.size() < 4 && rulev.size() > 0 ) {
            _errstr = "Rule parse error with line: " + ln;
            return false;
        } else if ( rulev.size() == 0 ) {
            continue;
        }


        if ( _debug ) {
            for ( vIter = rulev.begin(); vIter != rulev.end(); ++vIter )
                std::cout << *vIter << " ";
            std::cout << std::endl;
        }


        // rules in format of:
        //  
        //  [permit|deny]  proto  src  (srcport)  dst  (dstport)  flags
        //          0       1     2      3    ( 3 or 4) (4 or 5)  ( 4, 5, or 6)
        //
        FwRule  fwrule;
        int     i = 0;

        // permit or deny
        StringUtils::toLowerCase(rulev[i]);
        if ( rulev[i].compare("permit") == 0 )
            fwrule.permit = true;
        else if ( rulev[i].compare("deny") == 0 )
            fwrule.permit = false;
        else {       // not a rule
            std::cout << "NOT A RULE" << std::endl;
            continue;
        }
        i++;  // 1

        // proto
        if ( ! this->resolveProtocol(rulev[i], fwrule.proto) ) {
            _errstr = "Rule parse error in protocol: " + ln;
            return false;
        }
        i = 2;

        // src
        if ( StringUtils::startsWith(rulev[i], "$") ) {
            ln = rulev[i].substr(1);
            this->resolveFwVar(ln, fwrule.src);
        } else if ( CidrUtils::StringToCidr(rulev[i], fwrule.src) <= 0 ) {
            _errstr = "Rule parse error in source: " + ln;
            return false;
        }
        i = 3;
      
        // src port
        if ( fwrule.proto != _proto["ip"] ) {
            this->resolveFwPort(rulev[i], fwrule.srcport);
            i++; // 4
        }

        // dst   w/ i = 3 or 4
        if ( StringUtils::startsWith(rulev[i], "$") ) {
            ln = rulev[i].substr(1);
            this->resolveFwVar(ln, fwrule.dst);
        } else if ( CidrUtils::StringToCidr(rulev[i], fwrule.dst) <= 0 ) {
            _errstr = "Rule parse error in source: " + ln;
            return false;
        }
        i++;  // 4 or 5

        // dst port w/i = 4 or 5
        if ( fwrule.proto != _proto["ip"] ) {
            this->resolveFwPort(rulev[i], fwrule.dstport);
            i++; // 5 or 6
        }
        
        // flags 
        if ( (u_int) i < rulev.size() ) {
            std::string  flags = rulev[i];
            if ( StringUtils::startsWith(flags, "estab") )
                fwrule.established = true;
        }

        rulev.clear();
    }

    return true;
}


bool
FwRules::resolveProtocol ( const std::string & str, uint16_t & pval ) 
{
    std::string           protostr;
    FwProtoMap::iterator  pIter;

    protostr = StringUtils::trim(str);
    pIter    = _proto.find(protostr);

    if ( pIter == _proto.end() )
        return false;

    pval = pIter->second;

    return true;;
}

bool
FwRules::resolveFwVar ( const std::string & src, Prefix & srcPrefix )
{
    if ( _vars == NULL )
        return false;

    if ( ! _vars->exists(src) )
        return false;

    srcPrefix = _vars->find(src);

    return true;
}


bool
FwRules::resolveFwPort ( const std::string & portstr, FwPort & fwport )
{
    if ( portstr.empty() )
        return false;

    if ( portstr.compare("any") == 0 ) {
        fwport.port = 0;
        fwport. ranged = false;
        return true;
    }

    std::string             pstr = "";
    std::string::size_type  indx;

    indx = portstr.find_first_of(":");

    if ( indx == std::string::npos ) {
        fwport.port = StringUtils::fromString<uint16_t>(portstr);
        fwport.ranged = false;
    } else {
        pstr             = portstr.substr(0, indx);
        fwport.port      = StringUtils::fromString<uint16_t>(pstr);
        pstr             = portstr.substr(indx+1);
        fwport.port_high = StringUtils::fromString<uint16_t>(pstr);
        fwport.ranged    = true;
    }

    return true;
}

std::string
FwRules::getErrorStr() const 
{
    return this->_errstr;
}


bool
FwRules::parseProtoFile ( const std::string & protofile )
{
    std::string   ln;

    std::string::size_type              indx;
    std::vector<std::string>            protov;
    std::vector<std::string>::iterator  pIter;

    std::ifstream ifs(protofile.c_str(), std::ios_base::in);

    if ( this->_debug) 
        std::cout << "FwRules::parseProtocolFile(): " << protofile << std::endl;

    if ( ! ifs )
        return false;

    while ( std::getline(ifs, ln) ) {
        StringUtils::trim(ln);

        // ignoring comment
        if ( StringUtils::startsWith(ln, "#") || StringUtils::startsWith(ln, ";") )
            continue;

        // strip end of line comments
        indx = ln.find_first_of('#');
        if ( indx != std::string::npos )
            ln = ln.substr(0, indx);

        // replace tabs with whitespace
        while ( (indx = ln.find_first_of('\t')) != std::string::npos )
            ln.replace(indx, 1, "  ");

        StringUtils::split(ln, ' ', std::back_inserter(protov));

        if ( protov.size() == 0 ) {
            continue;
        } else if ( protov.size() < 3 ) {
            std::ostringstream  ostr;
            ostr << "Parse error in '" << protofile << "'. Invalid format in line '"
                 << ln << "'";
            _errstr = ostr.str();
            return false;
        }
        
        if ( this->_debug ) 
            std::cout << "Mapping '" << protov[0] << "' to '" 
                << protov[1] << "'" << std::endl;

        _proto[protov[0]] = StringUtils::fromString<uint16_t>(protov[1]);

        protov.clear();
    }

    return true;
}


} // namespace


// _FWGEN_FWRULES_CPP_

