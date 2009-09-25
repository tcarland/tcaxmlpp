#define _SOAP_CL_IMPL_CPP_

#include <string>

#include "soapH.h"



//--------------------------------------------------------------------//
// authenticate

int ns1__authenticate ( soap            * s,
                        xsd__string       username, 
		        xsd__string       password,
		        xsd__string       ipaddress,
		        ns1__AuthResponse & result )
{
    return SOAP_NO_METHOD;
}


int ns1__refresh ( soap         * s,
                   xsd__string    username,
                   xsd__string    ticket,
                   xsd__string    ipaddress,
                   xsd__boolean & result )
{
    return SOAP_NO_METHOD;
}


int ns1__expire ( soap         * s,
                  xsd__string    username,
                  xsd__string    ticket,
                  xsd__string    ipaddress,
                  xsd__boolean & result )
{
    return SOAP_NO_METHOD;
}


int ns1__isAuthentic ( soap         * s,
		       xsd__string    username,
		       xsd__string    ticket,
		       xsd__string    ipaddress,
		       xsd__boolean & result)
{
    return SOAP_NO_METHOD;
}

//--------------------------------------------------------------------//
//  Authorization


int ns1__authorized ( soap         * s,
		      xsd__string    username,
		      xsd__string    ticket,
		      xsd__string    ipaddress,
		      xsd__string    resourcename,
		      xsd__boolean & result)
{
    return SOAP_NO_METHOD;
}


int ns1__getAuthorizations ( soap        * s,
			     xsd__string   username,
			     xsd__string   ticket,
			     xsd__string   ipaddress,
			     struct ns1__getAuthorizationsResponse & result)
{
    return SOAP_NO_METHOD;
}



//--------------------------------------------------------------------//
//  Collectors


int ns1__getCollectorList ( soap * s,
                            xsd__string username,
                            xsd__string ticket,
                            xsd__string ipaddress,
                            struct ns1__getCollectorListResponse & result )
{    
    return SOAP_NO_METHOD;
}


//--------------------------------------------------------------------//
//  AuthTypes


int ns1__getAuthTypesList ( soap * s,
                            xsd__string username,
                            xsd__string ticket,
                            xsd__string ipaddress,
                            struct ns1__getAuthTypesListResponse & result )
{    
    return SOAP_NO_METHOD;
}



