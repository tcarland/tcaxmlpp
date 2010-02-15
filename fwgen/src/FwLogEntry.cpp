#define _FWGEN_FWLOGENTRY_CPP_

#include <iostream>


#include "FwLogEntry.h"

#include "StringUtils.h"
#include "CidrUtils.h"
using namespace tcanetpp;



namespace fwgen {



bool
FwLogEntry::ParseLogEntry ( const std::string & line, FwLogEntry & fwe )
{
    StringFields  fields;

    std::string   fwln, datehost, match;
    int           indx;
    uint32_t      nflds;

    match = FWLOG_KEYMATCH;
    nflds = FWLOG_FIELD_COUNT;
    indx  = StringUtils::indexOf(line, match);

    if ( indx < 0 )
        return false;

    fwln     = line;
    datehost = fwln.substr(0, indx);
    fwln.erase(0, indx + match.length());

    StringUtils::split(fwln, ' ', std::back_inserter(fields));

    if ( fields.size() < nflds ) {
        std::cout << "Line: '" << fwln << "'" << std::endl
            << "  has " << fields.size() << " fields, need " << nflds
            << std::endl;
    } else if ( fields.size() > nflds ) { //udp match
        FwLogEntry::SplitFields(fields);

        fwe.inf    = fields[0];
        fwe.outf   = fields[1];
        fwe.mac    = fields[2];
        fwe.src    = fields[3];
        fwe.dst    = fields[4];
        // len = 5, tos = 6, prec = 7, ttl = 8, id = 9
        
        int i = 10;
        if ( fields[i].compare("DF") == 0 )
            i++;
        fwe.proto  = fields[i++];
        fwe.spt    = fields[i++];
        fwe.dpt    = fields[i++];

        indx = StringUtils::lastIndexOf(datehost, " ");
        if ( indx > 0 ) {
            fwe.date = datehost.substr(0, indx);
            fwe.host = datehost.substr(indx+1);
        }

        std::cout << fwe.date << " " << fwe.host << " FW: IN=" 
                  << fwe.inf << " OUT=" << fwe.outf << " SRC=" 
                  << fwe.src << " DST=" << fwe.dst << " PROTO=" 
                  << fwe.proto  << " SPT=" << fwe.spt << " DPT=" 
                  << fwe.dpt << std::endl;
    } else
        return false;

    fields.clear();
    return true;
}



void
FwLogEntry::SplitFields ( StringFields & fields )
{
    StringFields::iterator  sIter;

    std::string  equal = "=";
    int          indx  = 0;

    for ( sIter = fields.begin(); sIter != fields.end(); ++sIter ) 
    {
        std::string & field = *sIter;

        if ( (indx = StringUtils::indexOf(field, equal)) > 0 )
            if ( field.length() >= ((u_int)indx + 1) )
                field.erase(0, indx+1);
    }

    return;
}


void
FwLogEntry::SetHostname ( FwLogEntry & fwe )
{
    ipv4addr_t   srcaddr;

    if ( CidrUtils::pton(fwe.src, srcaddr) == 1 ) {
        fwe.host = CidrUtils::GetHostName(srcaddr);
    }
}


} // namespace




