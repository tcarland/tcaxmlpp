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
