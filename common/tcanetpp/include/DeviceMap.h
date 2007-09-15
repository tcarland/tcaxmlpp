/**
  *  DeviceMap container of NetworkDevice objects.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  5.0
 **/
#ifndef _TCANETPP_DEVICEMAP_H_
#define _TCANETPP_DEVICEMAP_H_


#include <map>


namespace tcanetpp {

class NetworkDevice;


/**  The DeviceMap class acts as a container of NetworkDevice 
  *  objects. 
  *
  *  Essentially a wrapper of the stl::map indexing an IP 
  *  Address to a NetworkDevice object. The purpose of this
  *  wrapper is to provide a means to extend a set of attributes
  *  for groups of devices which could describe a variety of
  *  global parameters or device classification.
 **/
class DeviceMap {

  public:

    typedef std::map<ipv4addr_t, NetworkDevice*>    IpDeviceMap;
    typedef IpDeviceMap::iterator                   iterator;
    typedef IpDeviceMap::const_iterator             const_iterator;


  public:

    DeviceMap();

    virtual ~DeviceMap();
    

    NetworkDevice*  findDevice   ( ipv4addr_t addr );

    NetworkDevice*  addDevice    ( ipv4addr_t addr );
    NetworkDevice*  addDevice    ( const NetworkDevice & device );
    bool            addDevice    ( NetworkDevice * device );

    NetworkDevice*  removeDevice ( ipv4addr_t addr);

    bool            exists       ( ipv4addr_t addr );
    bool            exists       ( const NetworkDevice & device );

    void            clear(); 
    size_t          size() const  { return _devMap.size(); }
   
    /*  iterators */
    iterator        begin()       { return _devMap.begin(); }
    iterator        end()         { return _devMap.end(); }
    const_iterator  begin() const { return _devMap.begin(); }
    const_iterator  end() const   { return _devMap.end(); }
    

    void   erase  ( iterator & iter ) { _devMap.erase(iter); }
    void   erase  ( ipv4addr_t addr ) { _devMap.erase(addr); }
    void   erase  ( iterator & begin, 
                    iterator & end )  { _devMap.erase(begin, end); }
  

    void                groupId        ( uint32_t id );
    uint32_t            groupId();
    
    void                groupName      ( const std::string & name );
    const std::string&  groupName();
    
    void                groupData      ( void * group_data );
    void*               groupData();

    void                readCommunity  ( const std::string & rostr );
    const std::string&  readCommunity();
    
    void                writeCommunity ( const std::string & rwstr );
    const std::string&  writeCommunity();


    void                setDebug ( bool d ) { _debug = d; }


  private:
       
    IpDeviceMap                  _devMap;

    std::string                  _snmprw;
    std::string                  _snmpro;
    std::string                  _grpName;
    uint32_t                     _grpId;
    
    void*                        _grpData;
    bool   	                 _debug;
};

}  // namespace


#endif  // _TCANETPP_DEVICEMAP_H_
