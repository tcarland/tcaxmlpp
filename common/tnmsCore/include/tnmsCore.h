/*
 * tnmsCore.h
 *
 *  Copyright(c) 2008, 2009  Timothy Charlton Arland
 *      tca@charltontechnology.net
 *
 */
#ifndef _TNMSCORE_H_
#define _TNMSCORE_H_


//  global version info
#define TNMS_VERSION_MAJOR         0x0001
#define TNMS_VERSION_MINOR         0x0057
#define TNMS_VERSION               ( (((uint32_t)TNMS_VERSION_MAJOR << 16) & 0xffff0000) | \
                                      ((uint32_t)TNMS_VERSION_MINOR) )
#define TNMSCORE_VERSION           ( TNMS_VERSION )

#define TNMS_GET_MAJOR(x)          (  ((uint32_t)x >> 16) & 0x0000ffff )
#define TNMS_GET_MINOR(x)          (  (uint32_t)x & 0x0000ffff )


#define TNMS_DELIMITER             "/"
#define TNMS_DELIMITER_CHAR        '/'
#define TNMS_AGENT_ID              "tnmsagent"
#define TNMS_CLIENT_ID             "tnmsclient"


// client/protocol attributes
#define TNMS_RECORD_LIMIT          1000        // max records
#define TNMS_RECORD_SIZE           1024        // avg rec size

#define TNMS_PACKET_SIZE           ( TNMS_RECORD_SIZE * TNMS_RECORD_LIMIT )
#define TNMS_MAX_PACKET_SIZE       ( TNMS_PACKET_SIZE * 2 )

#define TNMS_COMPRESSION_ENABLE    0
#define TNMS_FLUSH_ENABLE          1

#define TNMS_LOGIN_ATTEMPTS        5
#define TNMS_LOGIN_INTERVAL        15

#define DEFAULT_TNMS_FLUSH_LIMIT           100         // minimum flush amt
#define DEFAULT_TNMS_CLIENT_TIMEOUT        120         // stall timeout

#define DEFAULT_TNMS_RECONFIG_INTERVAL     120
#define DEFAULT_TNMS_HOLDDOWN_INTERVAL     30
#define DEFAULT_TNMS_RECONNECT_INTERVAL    30



#include "tnmsProtocol.h"
#include "TnmsSocket.h"


#endif //  _TNMSCORE_H_

