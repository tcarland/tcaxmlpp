#define _SOURCE_FWRULES_CPP_

#include <fstream>
#include <iostream>
#include <vector>

#include "FwRules.h"
#include "FwVars.h"
#include "FwZones.h"

#include "Exception.hpp"
#include "StringUtils.h"
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
    std::string::size_type    indx;
    std::vector<std::string>  rulev;

    std::vector<std::string>::iterator  vIter;

    std::ifstream  ifn(rulefile.c_str());

    if ( ! ifn )
        return false;

    while ( std::getline(ifn, ln) ) {
        StringUtils::trim(ln);
 
        // ignoring comment
        if ( StringUtils::startsWith(ln, "#") || StringUtils::startsWith(ln, ";") )
            continue;

        // strip end of line comments
        indx = ln.find_first_of('#');
        if ( indx != std::string::npos )
            ln = ln.substr(0, indx);

        StringUtils::split(ln, ' ', std::back_inserter(rulev));

        if ( rulev.size() < 4 && rulev.size() > 0 ) {
            _errstr = "Rule parse error with line: " + ln;
            return false;
        } else if ( rulev.size() == 0 ) {
            continue;
        }

        FwRule  fwrule;

        if ( rulev[0].compare("permit") == 0 )
            fwrule.permit = true;
        else if ( rulev[0].compare("deny") == 0 )
            fwrule.permit = false;
        else {       // not a rule
            _errstr = "Rule parse error with line: " + ln;
            //return false;
            continue;
        }

        if ( _debug ) {
            for ( vIter = rulev.begin(); vIter != rulev.end(); ++vIter )
                std::cout << *vIter << " ";
            std::cout << std::endl;
        }

        // proto
        if ( ! this->parseProto(rulev[1], fwrule.proto) ) {
            _errstr = "Rule parse error in protocol: " + ln;
            return false;
        }

        // source
        if ( StringUtils::startsWith(rulev[2], "$") )
            this->resolveFwVar(rulev[2], fwrule.src);

        // src port
        if ( fwrule.proto != _proto["ip"] )
            this->resolveFwPort(rulev[3], fwrule.srcport);

        rulev.clear();
    }

    return true;
}

bool
FwRules::parseProto ( const std::string & str, uint16_t & pval ) 
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

        // replace any tabs with whitespace
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

// _SOURCE_FWRULES_CPP_
