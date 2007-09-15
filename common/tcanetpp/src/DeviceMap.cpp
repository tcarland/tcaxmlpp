/**  DeviceMap 
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.6
 **/
#define _TCANETPP_DEVICEMAP_CPP_


#include "DeviceMap.h"

#include "NetworkDevice.h"
#include "StringUtils.h"



namespace tcanetpp {


DeviceMap::DeviceMap()
    : _grpId(0),
      _grpData(NULL),
      _debug(false)
{}


DeviceMap::~DeviceMap()
{
    this->clear();
}

// ----------------------------------------------------------------------

NetworkDevice*
DeviceMap::findDevice ( ipv4addr_t addr )
{
    NetworkDevice       * device = NULL;
    DeviceMap::iterator   dIter;

    if ( (dIter = _devMap.find(addr)) != _devMap.end() )
	device = dIter->second;

    return device;
}

// ----------------------------------------------------------------------

NetworkDevice*
DeviceMap::addDevice ( ipv4addr_t addr )
{
    NetworkDevice *device = NULL;
    
    if ( (device = this->findDevice(addr)) == NULL ) {
	device = new NetworkDevice(addr);
	device->readCommunity(_snmpro);
	device->writeCommunity(_snmprw);
	_devMap[device->addr()] = device;
    }

    return device;
}


NetworkDevice*
DeviceMap::addDevice ( const NetworkDevice & device )
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
DeviceMap::addDevice ( NetworkDevice * device )
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
DeviceMap::removeDevice ( ipv4addr_t addr )
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
    if ( this->findDevice(addr) )
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

void
DeviceMap::groupId ( uint32_t id )
{
    _grpId = id;
}

uint32_t
DeviceMap::groupId()
{
    return _grpId;
}

// ----------------------------------------------------------------------

void
DeviceMap::groupName ( const std::string & name )
{
    _grpName = name;
}

const std::string&
DeviceMap::groupName()
{
    return _grpName;
}

// ----------------------------------------------------------------------

void
DeviceMap::groupData ( void * data )
{
    _grpData = data;
}

void*
DeviceMap::groupData()
{
    return _grpData;
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
