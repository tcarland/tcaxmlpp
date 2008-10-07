#define _SOURCE_FWVARS_CPP_

#include <fstream>

#include "StringUtils.h"
#include "CidrUtils.h"

#include "FwVars.h"


namespace fwgen {


FwVars::FwVars() 
{
}

FwVars::FwVars ( const std::string & varfile )
    throw ( Exception )
{
    if ( ! this->parse(varfile) )
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
    int                       indx = 0;
    Prefix                    pfx;

    std::ifstream  ifn(varfile.c_str());

    if ( !ifn )
        return false;

    while ( std::getline(ifn, ln) ) {
        StringUtils::trim(ln);
        
        // ignoring comment
        if ( StringUtils::startsWith(ln, "#") || StringUtils::startsWith(ln, ";") )
            continue;

        indx = StringUtils::indexOf(ln, "=");

        if ( indx < 0 )
            continue;

        key  = ln.substr(0, indx);
        val  = ln.substr(indx+1);
        StringUtils::trim(val);

        pfx = CidrUtils::StringToCidr(val);

        if ( ! this->insert(key, pfx) ) {
            this->_errStr = "Duplicate fw variable detected: " + key;
            return false;
        }
    }

    ifn.close();

    return true;
}


Prefix
FwVars::find ( const std::string & name )
{
    Prefix   p;

    FwVarMap::iterator  vIter = this->_varMap.find(name);

    if ( vIter == _varMap.end() )
        return p;

    p = vIter->second;

    return p;
}


bool
FwVars::insert ( const std::string & name, Prefix & p )
{
    if ( this->exists(name) )
        return false;

    this->_varMap[name] = p;
    return true;
}

bool
FwVars::insert ( const std::string & name, const Prefix & p )
{
    if ( this->exists(name) )
        return false;

    this->_varMap[name] = p;
    return true;
}


Prefix
FwVars::remove ( const std::string & name ) 
{
    Prefix             p;
    FwVarMap::iterator vIter = this->_varMap.find(name);

    if ( vIter == this->end() )
        return p;

    return vIter->second;
}

bool
FwVars::exists ( const std::string & name )
{
    FwVarMap::iterator vIter = this->_varMap.find(name);

    if ( vIter == this->end() )
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

// _SOURCE_FWVARS_CPP_

