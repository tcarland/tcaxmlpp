#ifndef _POLLAP_POLLERCONFIG_H_
#define _POLLAP_POLLERCONFIG_H_

#include <list>
#include <map>
#include <string>


#include "XmlDocument.h"
using namespace tca::xmlplus;

#include "PollerQuery.h"



namespace pollap {



class PollerConfig {

  public:

    PollerConfig()  {}
    virtual ~PollerConfig() {}


    bool         parseDeviceConfig  ( const std::string & filename,
                                      PollerDeviceList  & devices );

    bool         parseQueryConfig   ( const std::string & filename, 
                                      PollerQueryMap    & queries );
    
    static int   GetQueryType       ( const std::string & qtype );

    std::string  getErrorStr()      { return _errstr; }


  protected:
    
    bool         parseDeviceQueries ( XmlNode         * node,
                                      DeviceQueryMap  & devQueryMap );
    
    bool         parseQuerySection  ( XmlNode      * node, 
                                      PollerQuery  & rootQuery );

    void         parseQuery         ( XmlNode      * node, 
                                      PollerQuery  & query );

    void         parseResultMap     ( XmlNode      * node,
                                      PollerQuery  & query );

  private:

    std::string          _errstr;

};

}  // namespace

#endif  /*  _POLLAP_POLLERCONFIG_H_  */

