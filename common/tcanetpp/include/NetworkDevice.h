/** 
  * @file NetworkDevice.h
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
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
#ifndef _TCANETPP_NETWORKDEVICE_H_
#define _TCANETPP_NETWORKDEVICE_H_

#include <set>
#include <list>

#include "NetworkInterface.h"


namespace tcanetpp {

class NetworkDevice;

typedef std::set<NetworkDevice>    DeviceSet;
typedef std::list<NetworkDevice>   DeviceList;


class NetworkDevice {

  public:

    NetworkDevice();
    NetworkDevice  ( ipv4addr_t deviceip );
    NetworkDevice  ( const std::string & host );
    NetworkDevice  ( const std::string & host, const std::string & community );
    NetworkDevice  ( const NetworkDevice & device );

    virtual ~NetworkDevice();

    
    void    operator=   ( const NetworkDevice & device );
    bool    operator==  ( const NetworkDevice & device ) const;
    bool    operator<   ( const NetworkDevice & device ) const;
 
    
    bool                setDevice       ( const std::string & host );
    bool                setDevice       ( const ipv4addr_t  & addr );

    void                setDeviceName   ( const std::string & name );
    void                setDeviceAddr   ( const ipv4addr_t & addr );

    const std::string&  deviceName() const;
    const std::string&  name() const;

    const ipv4addr_t&   deviceAddr() const;
    const ipv4addr_t&   addr() const;

    void                deviceId       ( const int & id );
    const int&          deviceId() const;

    void                community      ( const std::string & comm );
    const std::string&  community() const;

    void                readCommunity  ( const std::string & comm );
    const std::string&  readCommunity() const;

    void                writeCommunity ( const std::string & comm );
    const std::string&  writeCommunity() const;

    void                version        ( const std::string & ver );
    const std::string&  version() const;

    void                sysDescr       ( const std::string & desc );
    const std::string&  sysDescr() const;

    void                model          ( const std::string & model );
    const std::string&  model() const;

    void                serialNumber   ( const std::string & serial );
    const std::string&  serialNumber() const;

    void                pollable       ( bool p );
    bool                pollable() const;

    int                 ifcount() const;

    void                status ( int s );
    const int&          status() const;

    void                errorStr       ( const std::string & err ); 
    std::string         errorStr() const;
 
    void                setNetworkInterfaces ( IfList & ifv );
    IfList&             getNetworkInterfaces();


  public:

    static 
    std::string         SetDeviceVersion ( NetworkDevice & dev, 
                                           const std::string & desc ); 

  private:

    IfList              _ifv;

    ipv4addr_t		_deviceAddr;
    int                 _deviceId;
    std::string	        _deviceName;
    
    std::string	        _readComm;
    std::string	        _writeComm;
    std::string	        _version;
    std::string	        _sysDescr;
    std::string	        _model;      
    std::string	        _location;
    std::string	        _serial;
    std::string	        _errStr;

    int                 _status;
    bool                _pollable;
    
};


}  // namespace

#endif  // _TCANETPP_NETWORKDEVICE_H_

