/**  NetworkDevice
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.5
 **/
#ifndef _TCANETPP_NETWORKDEVICE_H_
#define _TCANETPP_NETWORKDEVICE_H_

#include <set>
#include <list>

#include "NetworkInterface.h"


namespace tcanetpp {


class NetworkDevice;

typedef std::set<NetworkDevice>    DeviceSet;
typedef std::list<NetworkDevice>   DeviceList;



/**  NetworkDevice 
  *    An instantiation of the NetworkDevice class represents a
  *  single network host, such as a router, switch, firewall, etc.
  *  This can also include the physical or virtual interfaces
  *  of this device. The class is intended to be general enough
  *  for multiple uses and can be inherited to extend its use.
 **/
class NetworkDevice {

  public:

    NetworkDevice();
    NetworkDevice ( ipv4addr_t deviceip );
    NetworkDevice ( const std::string & host );
    NetworkDevice ( const std::string & host, const std::string & community );
    NetworkDevice ( const NetworkDevice & device );

    virtual ~NetworkDevice();

    
    void    operator=  ( const NetworkDevice & device );
    bool    operator== ( const NetworkDevice & device ) const;
    bool    operator<  ( const NetworkDevice & device ) const;

 
    
    // performs dns
    bool                setDevice ( const std::string & host );
    bool                setDevice ( const ipv4addr_t  & addr );

    // no dns
    void                setDeviceName ( const std::string & name );
    void                setDeviceAddr ( const ipv4addr_t & addr );

    /*  Accessors  */
    
    const std::string&  deviceName() const;
    const std::string&  name() const;
    
    const ipv4addr_t&   deviceAddr() const;
    const ipv4addr_t&   addr() const;

    void                deviceId ( const int & id );
    const int&          deviceId() const;

    void                community ( const std::string & comm );
    const std::string&  community() const;
    
    void                readCommunity  ( const std::string & comm );
    const std::string&  readCommunity() const;
    
    void                writeCommunity ( const std::string & comm );
    const std::string&  writeCommunity() const;

    void                version  ( const std::string & ver );
    const std::string&  version() const;

    void                sysDescr ( const std::string & desc );
    const std::string&  sysDescr() const;

    void                model   ( const std::string & model );
    const std::string&  model() const;

    void                serialNumber ( const std::string & serial );
    const std::string&  serialNumber() const;
    
    void                pollable ( bool p );
    bool                pollable() const;
    
    int                 ifcount() const;
    
    void                status ( int s );
    const int&          status() const;

    void                errorStr ( const std::string & err ); 
    std::string         errorStr() const;
 

    void                setNetworkInterfaces ( IfList & ifv );
    IfList&             getNetworkInterfaces();
    

    
  public:
      
    static 
    std::string         SetDeviceVersion ( NetworkDevice & dev, 
                                           const std::string & desc ); 


  private:

    IfList                  _ifv;

    ipv4addr_t		        _deviceAddr;
    int                     _deviceId;
    std::string	            _deviceName;
    
    std::string	  	        _readComm;
    std::string	  	        _writeComm;
    std::string	 	        _version;
    std::string	 	        _sysDescr;
    std::string	 	        _model;      
    std::string	 	        _location;
    std::string	 	        _serial;
    std::string	            _errStr;

    int                     _status;
    bool  		            _pollable;
    
};


}  // namespace


#endif  // _TCANETPP_NETWORKDEVICE_H_
