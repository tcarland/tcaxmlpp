#define _FWGEN_FWVARS_CPP_

#include <fstream>
#include <iostream>

#include "StringUtils.h"
using namespace tcanetpp;


#include "FwVars.h"


namespace fwgen {



FwVars::FwVars ( const std::string & varfile )
    throw ( Exception )
{
    if ( ! varfile.empty() && ! this->parse(varfile) )
        throw Exception(this->_errStr);
}

FwVars::~FwVars()
{
    this->clear();
}


bool
FwVars::parse ( const std::string & varfile )
{
    std::string               ln, key, val;
    FwVarMap::iterator        vIter;
    std::string::size_type    indx = 0;
    IpAddr                    pfx;

    std::ifstream  ifn(varfile.c_str());

    if ( !ifn )
        return false;

    while ( std::getline(ifn, ln) ) {
        StringUtils::trim(ln);
        StringUtils::stripComments(ln);
        StringUtils::replaceTabs(ln);

        // strip quotes
        while ( (indx = ln.find_first_of('\"')) != std::string::npos )
            ln.replace(indx, 1, " ");

        indx = ln.find_first_of('=');
        if ( indx == std::string::npos )
            continue;
        
        key = ln.substr(0, indx);
        val = ln.substr(indx + 1);
        StringUtils::trim(key);
        StringUtils::trim(val);

        if ( val.compare("any") == 0 ) {
            pfx = Prefix(0,0);
        } else if ( IpAddr::ToIpAddr(val, pfx) <= 0 ) {
            _errStr = "FwVars::parse() Error parsing address: " + val;
            return false;
        }

        if ( ! this->insert(key, pfx) ) {
            this->_errStr = "FwVars::parse() Duplicate fw variable detected: " 
                + key;
            return false;
        }
    }

    ifn.close();

    return true;
}


IpAddr
FwVars::find ( const std::string & name )
{
    IpAddr   p;

    FwVarMap::iterator  vIter = this->_varMap.find(name);

    if ( vIter == _varMap.end() )
        return p;

    p = vIter->second;

    return p;
}


bool
FwVars::insert ( const std::string & name, IpAddr & p )
{
    if ( this->exists(name) )
        return false;

    this->_varMap[name] = p;
    return true;
}

bool
FwVars::insert ( const std::string & name, const IpAddr & p )
{
    if ( this->exists(name) )
        return false;

    this->_varMap[name] = p;
    return true;
}


IpAddr
FwVars::remove ( const std::string & name ) 
{
    IpAddr   p;
    FwVarMap::iterator vIter;

    vIter = this->_varMap.find(name);

    if ( vIter == this->end() )
        return p;

    return vIter->second;
}

bool
FwVars::exists ( const std::string & name )
{
    FwVarMap::iterator vIter = this->_varMap.find(name);

    if ( vIter == _varMap.end() )
        return false;

    return true;
}

void
FwVars::clear()
{
    _varMap.clear();
    _errStr.clear();
}


size_t
FwVars::size() const
{
    return this->_varMap.size();
}


FwVars::iterator
FwVars::begin()
{
    return this->_varMap.begin();
}


FwVars::iterator
FwVars::end()
{
    return this->_varMap.end();
}


FwVars::const_iterator
FwVars::begin() const 
{
    return this->_varMap.begin();
}


FwVars::const_iterator
FwVars::end() const 
{
    return this->_varMap.end();
}


void
FwVars::erase ( iterator & iter )
{
    this->_varMap.erase(iter);
}


void
FwVars::erase ( const std::string & name ) 
{
    this->_varMap.erase(name);
}


void
FwVars::erase (  iterator & begin, iterator & end )
{
    return this->_varMap.erase(begin, end);
}


std::string
FwVars::getErrorStr() const
{
    return _errStr;
}



}  // namespace

// _FWGEN_FWVARS_CPP_

