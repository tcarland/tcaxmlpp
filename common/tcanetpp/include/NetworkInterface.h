/**
  * @file NetworkInterface.hpp
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
#ifndef _TCANETPP_NETWORKINTERFACE_H_
#define _TCANETPP_NETWORKINTERFACE_H_

#include <string>
#include <vector>


#include "tcanetpp_types.h"


namespace tcanetpp {


class NetworkInterface;
typedef std::vector<NetworkInterface>	IfList;


class NetworkInterface {
    
  public:

    NetworkInterface();
    NetworkInterface ( ipv4addr_t deviceip, int indx );
    NetworkInterface ( const NetworkInterface & nif );

    virtual ~NetworkInterface();


    virtual void operator=   ( const NetworkInterface & nif );
    virtual bool operator==  ( const NetworkInterface & nif );
    virtual bool operator<   ( const NetworkInterface & nif );


    int          ifIndex() const;

    void         ifName ( const std::string & ifname );
    std::string  ifName() const;

    void         description ( const std::string & desc );
    std::string  description() const;

    ipv4addr_t   deviceAddr()  const;

    void         ifAddr ( ipv4addr_t addr );
    ipv4addr_t   ifAddr() const;

    void         ifId   ( int id );
    int          ifId() const;


  private:

    ipv4addr_t		_device_ip;
    int			_if_index;
    ipv4addr_t		_if_addr;
    std::string 	_if_name;
    std::string		_description;
    std::string         _vlan;

    int                 _id;

};


} // namespace 


#endif  // _TCANETPP_NETWORKINTERFACE_H_

