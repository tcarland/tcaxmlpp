#ifndef _SOAP_DEFINITIONS_H_
#define _SOAP_DEFINITIONS_H_

#include <string.h>
#include <time.h>
#include <string>
#include <vector>


//gsoap ns1 service name: tnms
//gsoap ns1 service namespace: urn:tnms

/*  service port: http://localhost:8080  */

typedef char *   xsd__string;
typedef bool     xsd__boolean;
typedef int      xsd__int;
typedef time_t   xsd__dateTime;
typedef unsigned int xsd__unsignedInt;



//--------------------------------------------------------------------//
// authenticate


class ns1__AuthenticateResult 
{
  public:
    xsd__string         ticket;
    xsd__string         message;
    xsd__boolean        success;
    xsd__int            timeout;
};

struct ns1__AuthenticateResponse
{
    ns1__AuthResult     result;
};


int  ns1__authenticate  ( xsd__string       username,
                          xsd__string       password,
                          xsd__string       ipaddress,
                          ns1__AuthResult & result );

int  ns1__refresh       ( xsd__string       username,
                          xsd__string       ticket,
                          xsd__string       ipaddress,
                          xsd__boolean    & result);

int  ns1_expire         ( xsd__string       username,
                          xsd__string       ticket,
                          xsd__string       ipaddress,
                          xsd__boolean    & result );

int  ns1_isAuthentic    ( xsd__string       username,
                          xsd__string       ticket,
                          xsd__string       ipaddress,
                          xsd__boolean    & result );

//--------------------------------------------------------------------//
// authorize


struct ns1__AuthorizationList
{
    xsd__string *  ptr;
    xsd__int       size;
    xsd__boolean   isInclude;
};

struct ns1__getAuthorizationResponse
{
    ns1__AuthorizationList  result;
};


int  ns1__isAuthorized ( xsd__string    username,
                         xsd__string    ticket,
                         xsd__string    ipaddress,
                         xsd__string    resource,
                         xsd__boolean & result );

int  ns1__getAuthorizations ( xsd__string    username,
                              xsd__string    ticket,
                              xsd__string    ipaddress,
                              struct ns1__getAuthorizationResponse & result );


//--------------------------------------------------------------------//
//  Collectors


struct ns1__CollectorList
{
    xsd__string *  ptr;
    int            size;
};

struct ns1__getCollectorListResponse
{
    struct ns1__CollectorList   result;
};


int  ns1__getCollectorList  ( xsd__string  username,
                              xsd__string  ticket,
                              xsd__string  ipaddress,
                              struct ns1__getCollectorListResponse & result );


//--------------------------------------------------------------------//
//  Preferences


#import "stlvector.h"


struct ns1__Package
{
    xsd__string  pkg;
};

struct ns1__getPackageResponse
{
    struct ns1__Package _return;
};


class ns1__PackageInfo
{
  public:
    xsd__string     owner;
    xsd__string     name;
    xsd__string     desc;
    xsd__string     version;
    xsd__dateTime   timestamp;
    xsd__boolean    is_public;
};


class ns1__getPackageListResponse
{
  public:
    std::vector<ns1__PackageInfo> *entries;
};


int  ns1__getPackage    ( xsd__string  username,
                          xsd__string  ticket,
                          xsd__string  ipaddress,
                          xsd__string  package_owner,
                          xsd__string  package_name,
                          struct ns1__getPackageResponse & result );

int ns1__submitPackage  ( xsd__string   username,
                          xsd__string   ticket,
                          xsd__string   ipaddress,
                          xsd__string   package_name,
                          xsd__string   package_desc,
                          xsd__string   package_vers,
                          xsd__dateTime package_time,
                          xsd__string   package,
                          xsd__boolean  is_public,
                          xsd__boolean & result );

int ns1__getPackages    ( xsd__string  username,
                          xsd__string  ticket,
                          xsd__string  ipaddress,
                          ns1__getPackageListResponse & result );

int ns1__removePackage  ( xsd__string  username,
                          xsd__string  ticket,
                          xsd__string  ipaddress,
                          xsd__string  package_name,
                          xsd__boolean & result );

//--------------------------------------------------------------------//


#endif  // _SOAP_DEFINITIONS_H_

