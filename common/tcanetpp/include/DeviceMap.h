/**
  *  DeviceMap is a container of NetworkDevice objects.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  5.0
 **/
#ifndef _TCANETPP_DEVICEMAP_H_
#define _TCANETPP_DEVICEMAP_H_

#include <string>
#include <map>

#include "tcanetpp_types.h"


namespace tcanetpp {

class NetworkDevice;


class DeviceMap {

  public:

    typedef std::map<ipv4addr_t, NetworkDevice*>    IpDeviceMap;
    typedef IpDeviceMap::iterator                   iterator;
    typedef IpDeviceMap::const_iterator             const_iterator;


  public:

    DeviceMap();

    virtual ~DeviceMap();
    

    NetworkDevice*      findDevice     ( ipv4addr_t addr );

    NetworkDevice*      addDevice      ( ipv4addr_t addr );
    NetworkDevice*      addDevice      ( const NetworkDevice & device );
    bool                addDevice      ( NetworkDevice * device );

    NetworkDevice*      removeDevice   ( ipv4addr_t addr);

    bool                exists         ( ipv4addr_t addr );
    bool                exists         ( const NetworkDevice & device );

    void                clear(); 
    size_t              size()  const;
   
    iterator            begin();
    iterator            end(); 
    const_iterator      begin() const;
    const_iterator      end()   const;
    

    void                erase          ( iterator & iter );
    void                erase          ( ipv4addr_t addr );
    void                erase          ( iterator & begin,  iterator & end );

    void                groupId        ( uint32_t id );
    uint32_t            groupId();
    
    void                groupName      ( const std::string & name );
    const std::string&  groupName();
    
    void                readCommunity  ( const std::string & rostr );
    const std::string&  readCommunity();
    
    void                writeCommunity ( const std::string & rwstr );
    const std::string&  writeCommunity();


    void                setDebug       ( bool d );


  private:
       
    IpDeviceMap                  _devMap;

    std::string                  _snmprw;
    std::string                  _snmpro;
    std::string                  _groupName;
    uint32_t                     _groupId;
    
    bool   	                 _debug;
};

}  // namespace


#endif  // _TCANETPP_DEVICEMAP_H_
