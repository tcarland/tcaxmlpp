/**  NetworkDevice
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
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
#define _TCANETPP_NETWORKDEVICE_CPP_

extern "C" {
#ifndef WIN32
# include <netdb.h>
# include <unistd.h>
# include <sys/socket.h>
#endif
}
#include <vector>

#include "NetworkDevice.h"

#include "StringUtils.h"
#include "CidrUtils.h"


namespace tcanetpp {


NetworkDevice::NetworkDevice()
    : _deviceAddr(0),
      _deviceId(0),
      _pollable(true)
{}

NetworkDevice::NetworkDevice ( ipv4addr_t deviceip )
    : _deviceAddr(0),
      _deviceId(0),
      _pollable(true)
{
    this->setDevice(CidrUtils::ntop(deviceip));
}

NetworkDevice::NetworkDevice ( const std::string & host )
    : _deviceAddr(0),
      _deviceId(0),
      _pollable(true)
{
    this->setDevice(host);
}


NetworkDevice::NetworkDevice ( const std::string & host, const std::string & comm )
    : _deviceAddr(0),
      _deviceId(0),
      _readComm(comm),
      _pollable(true)
{
    this->setDevice(host);
}

NetworkDevice::NetworkDevice ( const NetworkDevice & device )
    : _deviceAddr(device._deviceAddr),
      _deviceId(device._deviceId),
      _deviceName(device._deviceName),
      _readComm(device._readComm),
      _writeComm(device._writeComm),
      _version(device._version),
      _sysDescr(device._sysDescr),
      _model(device._model),
      _location(device._location),
      _serial(device._serial),
      _status(device._status),
      _pollable(device._pollable)
{}

NetworkDevice::~NetworkDevice()
{}

// ----------------------------------------------------------------------

void
NetworkDevice::operator= ( const NetworkDevice & device )
{
    _ifv        = device._ifv;
    _deviceAddr = device._deviceAddr;
    _deviceId   = device._deviceId;
    _deviceName = device._deviceName;
    _readComm   = device._readComm;
    _writeComm  = device._writeComm;
    _version    = device._version;
    _sysDescr   = device._sysDescr;
    _model      = device._model;
    _location   = device._location;
    _serial     = device._serial;
    _status     = device._status;
    _pollable   = device._pollable;
}


bool
NetworkDevice::operator== ( const NetworkDevice & device ) const
{
    return(this->_deviceAddr == device._deviceAddr);
}


bool
NetworkDevice::operator< ( const NetworkDevice & device ) const
{
    return(this->deviceAddr() < device.deviceAddr());
}


// ----------------------------------------------------------------------

/**@{
  *   Returns this objects device hostname or IP address 
  *   accordingly
 **/
const std::string&
NetworkDevice::deviceName() const
{
    return this->_deviceName;
}

const std::string&
NetworkDevice::name() const
{
    return this->deviceName();
}

const ipv4addr_t&
NetworkDevice::deviceAddr() const
{
    return this->_deviceAddr;
}

const ipv4addr_t&
NetworkDevice::addr() const
{
    return this->deviceAddr();
}
/*@}*/

/**@{
  *   A device ID is really best considered it's IP, but this 
  *   is left general enough to be multipurpose. Could be used in 
  *   context of a node or database id.
 **/
void
NetworkDevice::deviceId ( const int & id )
{
    this->_deviceId = id;
}

const int&
NetworkDevice::deviceId() const
{
    return this->_deviceId;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{  Sets or returns the snmp community string.  */
void
NetworkDevice::readCommunity ( const std::string & comm ) 
{
    this->_readComm = comm;
}

const std::string&
NetworkDevice::readCommunity() const
{
    return this->_readComm;
}

void
NetworkDevice::writeCommunity ( const std::string & comm )
{
    this->_writeComm = comm;
}

const std::string&
NetworkDevice::writeCommunity() const
{
    return this->_writeComm;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the device version string.  */
void
NetworkDevice::version ( const std::string & ver )
{
    this->_version = ver;
}

const std::string&
NetworkDevice::version() const
{
    return this->_version;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the device's system description string.  */
const std::string&
NetworkDevice::sysDescr() const
{
    return this->_sysDescr;
}


void
NetworkDevice::sysDescr ( const std::string & desc )
{
    this->_sysDescr = desc;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the device platform string. */
void
NetworkDevice::model ( const std::string & model )
{
    this->_model = model;
}

const std::string&
NetworkDevice::model() const
{
    return this->_model;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{  Set/Get for the device serial number.  */
void
NetworkDevice::serialNumber ( const std::string & serial )
{
    this->_serial = serial;
}


const std::string&
NetworkDevice::serialNumber() const
{
    return this->_serial;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Get/Set method to intended to provide a means to temporarily disable 
 *    polling or querying a device.
 **/
void
NetworkDevice::pollable ( bool p )
{
    this->_pollable = p;
}

bool
NetworkDevice::pollable() const
{
    return this->_pollable;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{
  *   The status methods allow for application specific use
  *   of error states or other device processing states.
 **/
void
NetworkDevice::status ( int s )
{
    this->_status = s;
}


const int&
NetworkDevice::status() const
{
    return this->_status;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the internal error string. */
void
NetworkDevice::errorStr ( const std::string & err )
{
    this->_errStr = err;
}


std::string
NetworkDevice::errorStr() const
{
    return this->_errStr;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{
  *   These methods attempt to interpret the hostname string parameter 
  *  provided in CDP queries. This is generally of the format of 
  *  "hostname" (or fqdn) or "serial#(hostname)". This method will 
  *  perform forward or reverse dns lookups depending on what format 
  * is provided.
 **/
bool
NetworkDevice::setDevice ( const std::string & host )
{
    struct hostent * hp   = NULL;
    ipv4addr_t       addr = 0;
    int              indx = 0;
    int              endx = 0;
    std::string      hoststr;

    // check for a hostaddr in string format
    if ( CidrUtils::StringToAddr(host, addr) ) {
	return this->setDevice(addr);
	
    //  this is very CDP specific which often returns 'serial_num (host)'
    } else if ( (indx = StringUtils::indexOf(host, "(")) > 0 ) {
	_serial     = host.substr(0, indx);
	_deviceName = host.substr((indx + 1), host.length());
	
	if ( (endx = StringUtils::indexOf(_deviceName, ")")) > 0 )
	    _deviceName.erase(endx);

    } else {
	_deviceName = host;
    }

    // resolve hostname if we don't have ip yet
    if ( addr == 0 ) {
	if ( (indx = StringUtils::indexOf(_deviceName, ".")) > 0 )
	    _deviceName = _deviceName.substr(0, indx);

	hp = ::gethostbyname(_deviceName.c_str());

	if ( ! hp )
	    return false;

	_deviceName = hp->h_name;
	_deviceAddr = *((uint32_t*)hp->h_addr);
    }

    return true;
}


bool
NetworkDevice::setDevice ( const ipv4addr_t & addr )
{
    this->_deviceAddr = addr;
    this->_deviceName = CidrUtils::GetHostName(addr);

    if ( _deviceName.empty() ) {
	_deviceName = CidrUtils::ntop(_deviceAddr);
        return false;
    }

    return true;
}
/*@}*/


/**@{
  *   Sets device name or address without performing dns resolution.
 **/
void         
NetworkDevice::setDeviceName ( const std::string & name )
{
    this->_deviceName = name;
}

void
NetworkDevice::setDeviceAddr ( const ipv4addr_t & addr )
{
    this->_deviceAddr = addr;
}
/*@}*/

// ----------------------------------------------------------------------

/**   Returns the number of interfaces as stored by the
  *   setNetworkInterfaces() method.
 **/
int
NetworkDevice::ifcount() const
{
    return this->_ifv.size();
}

// ----------------------------------------------------------------------

/**@{ 
  *   These set/get methods allow a NetworkDevice object to
  *   contain the related vector of NetworkInterface objects
  *   for this device.
 **/
void
NetworkDevice::setNetworkInterfaces ( IfList & ifv )
{
    this->_ifv = ifv;
}

IfList&
NetworkDevice::getNetworkInterfaces()
{
    return this->_ifv;
}
/*@}*/

// ----------------------------------------------------------------------

std::string
NetworkDevice::SetDeviceVersion ( NetworkDevice & dev, const std::string & desc )
{
    std::string  ver, vs;
    std::vector<std::string>  stok;
    
    StringUtils::split(desc, '\n', std::back_inserter(stok));
    
    if ( stok.size() >= 2 ) {
	vs   = stok[1];
	stok.clear();
	
	StringUtils::split(vs, ',', std::back_inserter(stok));
	vs = "";
	if ( stok.size() >= 2 ) {
	    vs   = stok[0];
	    ver  = stok[1];
	}
    } else {
	ver  = desc;
    }

    StringUtils::trim(ver);

    if ( dev.deviceAddr() > 0 ) {

	dev.sysDescr(desc);
	dev.version(ver);

	/* old ios detection stuff
	if ( vs.length() == 0 )
	    dev.ios(0);
	else if ( StringUtils::indexOf(vs, "IOS") >= 0 )
	    dev.ios(1);
	else
	    dev.ios(-1);
	 */
    }

    return ver;
}

// ----------------------------------------------------------------------

} // namespace


// _TCANETPP_NETWORKDEVICE_CPP_

