/**  NetworkDevice
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.5
 **/
#define _TCANETPP_NETWORKDEVICE_CPP_

extern "C" {
# include <netdb.h>
# include <unistd.h>
# include <sys/socket.h>
}

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
    return(_deviceAddr == device._deviceAddr);
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
    return _deviceName;
}

const ipv4addr_t&
NetworkDevice::deviceAddr() const
{
    return _deviceAddr;
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
    _deviceId = id;
}

const int&
NetworkDevice::deviceId() const
{
    return _deviceId;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{  Sets or returns the snmp community string.  */
void
NetworkDevice::readCommunity ( const std::string & comm ) 
{
    _readComm = comm;
}

const std::string&
NetworkDevice::readCommunity() const
{
    return _readComm;
}

void
NetworkDevice::writeCommunity ( const std::string & comm )
{
    _writeComm = comm;
}

const std::string&
NetworkDevice::writeCommunity() const
{
    return _writeComm;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the device version string.  */
void
NetworkDevice::version ( const std::string & ver )
{
    _version = ver;
}

const std::string&
NetworkDevice::version() const
{
    return _version;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the device's system description string.  */
const std::string&
NetworkDevice::sysDescr() const
{
    return _sysDescr;
}


void
NetworkDevice::sysDescr ( const std::string & desc )
{
    _sysDescr = desc;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the device platform string. */
void
NetworkDevice::model ( const std::string & model )
{
    _model = model;
    if ( StringUtils::indexOf(model, "3548") >= 0 )
        _crippled = true;
}

const std::string&
NetworkDevice::model() const
{
    return _model;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{  Set/Get for the device serial number.  */
void
NetworkDevice::serialNumber ( const std::string & serial )
{
    _serial = serial;
}


const std::string&
NetworkDevice::serialNumber() const
{
    return _serial;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Get/Set method to intended to provide a means to temporarily disable 
 *    polling or querying a device.
 **/
void
NetworkDevice::pollable ( bool p )
{
    _pollable = p;
}

bool
NetworkDevice::pollable() const
{
    return _pollable;
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
    _status = s;
}


const int&
NetworkDevice::status() const
{
    return _status;
}
/*@}*/

// ----------------------------------------------------------------------

/**@{ Sets or returns the internal error string. */
void
NetworkDevice::errorStr ( const std::string & err )
{
    _errStr = err;
}


std::string
NetworkDevice::errorStr() const
{
    return _errStr;
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

	hp = gethostbyname(_deviceName.c_str());

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
    struct hostent * hp;

    _deviceAddr = addr;

    hp = gethostbyaddr(&_deviceAddr, sizeof(_deviceAddr), AF_INET);

    if ( hp ) {
	_deviceName = hp->h_name;
    } else {
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
    _deviceName = name;
}

void
NetworkDevice::setDeviceAddr ( const ipv4addr_t & addr )
{
    _deviceAddr = addr;
}
/*@}*/

// ----------------------------------------------------------------------

/**   Returns the number of interfaces as stored by the
  *   setNetworkInterfaces() method.
 **/
int
NetworkDevice::ifcount() const
{
    return _ifv.size();
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
    _ifv = ifv;
    return;
}

IfList&
NetworkDevice::getNetworkInterfaces()
{
    return _ifv;
}
/*@}*/

// ----------------------------------------------------------------------

std::string
NetworkDevice::SetDeviceVersion ( NetworkDevice & dev, const std::string & desc )
{
    std::string  ver, vs;
    std::vector  stok;
    
    StringUtils::split(desc, "\n", std::back_inserter(stok));
    

    if ( stok.size() >= 2 ) {
	vs   = stok[1];
	stok.clear();
	
	StringUtils::split(vs, ",", std::back_inserter(stok));
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