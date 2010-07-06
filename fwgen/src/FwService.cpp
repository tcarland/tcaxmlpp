#define _FWGEN_FWSERVICE_CPP_ 

#include <iostream>
#include <fstream>
#include <vector>

#include "FwService.h"

#include "StringUtils.h"
using namespace tcanetpp;


namespace fwgen {


bool
FwService::ParseServices ( const std::string & svcfile, FwSvcMap & svcmap, bool debug )
{
    std::ifstream        ifs;

    ifs.open(svcfile.c_str(), std::ios::in);

    if ( ! ifs ) {
        std::cout << "Error opening: " << svcfile << std::endl;
        return false;
    } else
        std::cout << "Parsing services file: " << svcfile << std::endl;


    char line[BIGSTRLINE];
    std::string  svcline;
    int          indx;

    std::vector<std::string>  fields;

    while ( ifs.getline(line, BIGSTRLINE) )
    {
        svcline  = line;

        StringUtils::trim(svcline);
        if ( StringUtils::startsWith(svcline, "#") )
            continue;

        fields.clear();
        StringUtils::split(svcline, '\t', std::back_inserter(fields));

        if ( fields.size() >= 2 && ! fields[0].empty() )
        {
            FwService   svc;

            svc.name   = fields[0];
            svc.proto  = fields[1];
            if ( (indx = StringUtils::indexOf(svc.proto, "/")) < 0 )
                continue;
            svc.port   = StringUtils::fromString<int>(svc.proto.substr(0, indx));
            if ( svc.port == 0 )
                continue;
            svc.proto = svc.proto.substr(indx+1);

            if ( fields.size() == 4 )
                svc.desc = fields[3];
            else if ( fields.size() == 3 )
                svc.desc = fields[2];

            if ( (indx = StringUtils::indexOf(svc.desc, "#")) < 0 ) {
                svc.desc = "";
            } else {
                svc.desc = svc.desc.substr(indx+1);
                StringUtils::trim(svc.desc);
            }

            if ( debug ) {
	            std::cout << " :" << svc.name << ":" << svc.port << ":" << svc.proto;
	            if ( ! svc.desc.empty() )
	                std::cout << ":" << svc.desc;
	            std::cout << std::endl;
            }

            svcmap[svc.port] = svc;
        }
    }

    ifs.close();


    if ( svcmap.empty() )
        return false;

    return true;
}


} // namespace

#ifdef FWSERVICE_TEST

int main()
{
    FwSvcMap     map;
    std::string  svcs = ETC_SERVICES;

    FwService::ParseServices(svcs, svcmap, true);

    return 0;
}


#endif

// _FWGEN_FWSERVICE_CPP_

