#ifndef _FWGEN_FWSERVICE_H_
#define _FWGEN_FWSERVICE_H_

#include <map>
#include <string>


namespace fwgen {

#define ETC_SERVICES "/etc/services"

class FwService;



typedef std::map<int, FwService> FwSvcMap;


class FwService {

  public:

    int          port;
    std::string  name;
    std::string  proto;
    std::string  desc;



    FwService() : port(0) {}
    ~FwService() {}


    static bool  ParseServices ( const std::string & svcfile, FwSvcMap & svcmap );

};


} // namespace

#endif  // _FWGEN_FWSERVICE_H_

