#ifndef _TNMSCORE_H_
#define _TNMSCORE_TNMSCONFIG_H_

#include "tnmsProtocol.h"
#include "TnmsSocket.h"

#define TNMS_VERSION_MAJOR          0x05
#define TNMS_VERSION_MINOR          0x61

#define TNMS_DEFAULT_DELIMITER      "."
#define TNMS_RECORD_LIMIT           1000   // max records
#define TNMS_RECORD_SIZE            1024   // avg rec size
#define TNMS_PACKET_SIZE            (TNMS_RECORD_SIZE * TNMS_RECORD_LIMIT)
#define TNMS_MAX_PACKET_SIZE        (TNMS_PACKET_SIZE * 2)

#define TNMS_FLUSH_LIMIT            500   // minimum flush amt
#define TNMS_CLIENT_TIMEOUT         120   // stall timeout

#define COMPRESSION_ENABLE          1
#define FLUSH_ENABLE                1




#endif //  _TNMSCORE_H_
