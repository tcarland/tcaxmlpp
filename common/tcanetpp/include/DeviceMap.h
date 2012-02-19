/**
  * @file DeviceMap.h
  *
  *    A container of NetworkDevice objects.
  * 
  * Copyright (c) 2002,2008 Timothy Charlton Arland 
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#ifndef _TCANETPP_DEVICEMAP_H_
#define _TCANETPP_DEVICEMAP_H_

#include <string>
#include <map>

#include "tcanetpp_types.h"


namespace tcanetpp {


class NetworkDevice;


/**  A map of host ip to NetworkDevice object with an interface 
  * extending an STL map.
 **/
class DeviceMap {

  public:

    typedef std::map<ipv4addr_t, NetworkDevice*>    IpDeviceMap;
    typedef IpDeviceMap::iterator                   iterator;
    typedef IpDeviceMap::const_iterator             const_iterator;


  public:

    DeviceMap();

    virtual ~DeviceMap();
    

    NetworkDevice*      find     ( ipv4addr_t addr );
    NetworkDevice*      insert   ( ipv4addr_t addr );
    NetworkDevice*      insert   ( const NetworkDevice & device );
    bool                insert   ( NetworkDevice * device );
    NetworkDevice*      remove   ( ipv4addr_t addr);

    bool                exists   ( ipv4addr_t addr );
    bool                exists   ( const NetworkDevice & device );

    void                clear(); 
    size_t              size()  const;
   
    iterator            begin();
    iterator            end(); 
    const_iterator      begin() const;
    const_iterator      end()   const;
    

    void                erase    ( iterator & iter );
    void                erase    ( ipv4addr_t addr );
    void                erase    ( iterator & begin,  iterator & end );

    void                groupId  ( uint32_t id );
    uint32_t            groupId();
    
    void                groupName      ( const std::string & name );
    const std::string&  groupName();
    
    void                readCommunity  ( const std::string & ro_str );
    const std::string&  readCommunity();
    
    void                writeCommunity ( const std::string & rw_str );
    const std::string&  writeCommunity();


    void                setDebug ( bool d );


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

