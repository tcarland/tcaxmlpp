/**
  * @file DeviceMap.cpp
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author  tca@charltontechnology.net
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
#define _TCANETPP_DEVICEMAP_CPP_

#include "DeviceMap.h"

#include "NetworkDevice.h"
#include "StringUtils.h"


namespace tcanetpp {


// ----------------------------------------------------------------------

DeviceMap::DeviceMap()
    : _groupId(0),
      _debug(false)
{}


DeviceMap::~DeviceMap()
{
    this->clear();
}

// ----------------------------------------------------------------------

NetworkDevice*
DeviceMap::find ( ipv4addr_t addr )
{
    NetworkDevice       * device = NULL;
    DeviceMap::iterator   dIter;

    if ( (dIter = _devMap.find(addr)) != _devMap.end() )
	device = dIter->second;

    return device;
}

// ----------------------------------------------------------------------

NetworkDevice*
DeviceMap::insert ( ipv4addr_t addr )
{
    NetworkDevice       * dev = NULL;
    DeviceMap::iterator   dIter;
    
    if ( (dIter = _devMap.find(addr)) == _devMap.end() ) {
	dev = new NetworkDevice(addr);
	dev->readCommunity(_snmpro);
	dev->writeCommunity(_snmprw);
	_devMap[dev->addr()] = dev;
    }

    return dev;
}


NetworkDevice*
DeviceMap::insert ( const NetworkDevice & device )
{
    NetworkDevice       * dev = NULL;
    DeviceMap::iterator   dIter;

    if ( (dIter = _devMap.find(device.addr())) == _devMap.end() ) {
        dev = new NetworkDevice(device);
	_devMap[device.addr()] = dev;
    }

    return dev;
}


bool
DeviceMap::insert ( NetworkDevice * device )
{
    DeviceMap::iterator  dIter;

    if ( (dIter = _devMap.find(device->addr())) == _devMap.end() )
	_devMap[device->addr()] = device;
    else
	return false;

    return true;
}

// ----------------------------------------------------------------------

NetworkDevice*
DeviceMap::remove ( ipv4addr_t addr )
{
    NetworkDevice      * device = NULL;
    DeviceMap::iterator  dIter;

    if ( (dIter = _devMap.find(addr)) != _devMap.end() ) {
	device = (NetworkDevice*) dIter->second;
	_devMap.erase(dIter);
    }

    return device;
}

// ----------------------------------------------------------------------

bool
DeviceMap::exists ( ipv4addr_t addr )
{
    if ( this->find(addr) )
	return true;

    return false;
}


bool
DeviceMap::exists ( const NetworkDevice & device )
{
    return this->exists(device.addr());
}

// ----------------------------------------------------------------------

void
DeviceMap::clear()
{
    DeviceMap::iterator dIter;

    for ( dIter = _devMap.begin(); dIter != _devMap.end(); dIter++ )
	if ( dIter->second )
	    delete dIter->second;

    _devMap.clear();
    return;
}

// ----------------------------------------------------------------------

size_t
DeviceMap::size() const
{
    return _devMap.size();
}

// ----------------------------------------------------------------------

DeviceMap::iterator
DeviceMap::begin()
{
    return _devMap.begin();
}


DeviceMap::iterator
DeviceMap::end()
{
    return _devMap.end();
}


DeviceMap::const_iterator
DeviceMap::begin() const
{
    return _devMap.begin();
}


DeviceMap::const_iterator
DeviceMap::end() const
{
    return _devMap.end();
}

// ----------------------------------------------------------------------

void
DeviceMap::erase ( iterator & iter )
{
    _devMap.erase(iter);
}


void
DeviceMap::erase ( ipv4addr_t addr )
{
    _devMap.erase(addr);
}


void
DeviceMap::erase ( iterator & begin, iterator & end )
{
    _devMap.erase(begin, end);
}

// ----------------------------------------------------------------------

void
DeviceMap::groupId ( uint32_t id )
{
    _groupId = id;
}

uint32_t
DeviceMap::groupId()
{
    return _groupId;
}

// ----------------------------------------------------------------------

void
DeviceMap::groupName ( const std::string & name )
{
    _groupName = name;
}

const std::string&
DeviceMap::groupName()
{
    return _groupName;
}

// ----------------------------------------------------------------------

void
DeviceMap::writeCommunity ( const std::string & rwstr )
{
    _snmprw = rwstr;
}


const std::string&
DeviceMap::writeCommunity()
{
    return _snmprw;
}

// ----------------------------------------------------------------------

void
DeviceMap::readCommunity ( const std::string & rostr )
{
    _snmpro = rostr;
}


const std::string&
DeviceMap::readCommunity()
{
    return _snmpro;
}

// ----------------------------------------------------------------------


} // namespace

//  _TCANETPP_DEVICEMAP_CPP_

