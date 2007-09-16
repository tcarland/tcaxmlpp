#ifndef OIDS_H_
#define OIDS_H_


//
//  standard MIB-2 oids
//
// 'sysObjectID'         "1.3.6.1.2.1.1.2.0"  
// 'sysDescr'            "1.3.6.1.2.1.1.1.0"  string
// 'sysUpTime'           "1.3.6.1.2.1.1.3.0"  timeticks
// 'sysName'             "1.3.6.1.2.1.1.5.0"  string
#define SYSOBJECT_OID    "sysObjectID.0"
#define SYSDESCR_OID     "sysDescr.0"
#define SYSUPTIME_OID    "sysUpTime.0"
#define SYSNAME_OID      "sysName.0"


//
//  Interface OIDS
//
//"ifIndex"              "1.3.6.1.2.1.2.2.1.1"  integer
//"ifDescr"              "1.3.6.1.2.1.2.2.1.2"  string
//"ifAlias"              "1.3.6.1.2.1.31.1.1.1.18" string
#define IFINDEX_OID      "interfaces.ifTable.ifEntry.ifIndex"
#define IFDESCR_OID      "interfaces.ifTable.ifEntry.ifDescr"
#define IFALIAS_OID      "ifMIB.ifMIBObjects.ifXTable.ifXEntry.ifAlias"

//"ipAdEntAddr"          "1.3.6.1.2.1.4.20.1.1"
//"ipAdEntIfIndex"       "1.3.6.1.2.1.4.20.1.2"
//"ipAdEntNetMask"       "1.3.6.1.2.1.4.20.1.3"
#define IP_ADDRENTRY_OID "ip.ipAddrTable.ipAddrEntry.ipAdEntAddr"
#define IP_INDXENTRY_OID "ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex"
#define IP_MASKENTRY_OID "ip.ipAddrTable.ipAddrEntry.ipAdEntNetMask"


//
//  Cisco Chassis OIDS
// 
//   Switch/Catalyst oids from CISCO-STACK-MIB
//   Router/IOS oids from OLD-CISCO-CHASSIS-MIB
//
//"chassis id"           "1.3.6.1.4.1.9.3.6.3.0"    string
//"stack switch id"      "1.3.6.1.4.1.9.5.1.2.19.0" string
#define CHASSIS_SERIAL_OID       "enterprises.9.3.6.3.0"
#define STACK_SERIAL_OID         "enterprises.9.5.1.2.19.0"

//chassis model id       "1.3.6.1.4.1.9.3.6.1.0"    integer
//stack  model nam3      "1.3.6.1.4.1.9.5.1.2.16.0" string
#define CHASSIS_MODELTYPE_OID    "enterprises.9.3.6.1.0"
#define STACK_MODELNAME_OID      "enterprises.9.5.1.2.16.0"

//
//  Mib-2  ENTITY MIB
//
//   Some items to replace deprecated OLD-CISCO-CHASSIS-MIB
//
#define ENTITY_CHASSISVER_OID    "mib-2.47.1.1.1.1.9.1"
#define ENTITY_CHASSISID_OID     "mib-2.47.1.1.1.1.11.1"
#define ENTITY_VENDORNAME_OID    "mib-2.47.1.1.1.1.12.1"
#define ENTITY_MODELNAME_OID     "mib-2.47.1.1.1.1.13.1"

//  Logical vlan table 
#define ENTITY_LOGICALDESC_OID   "mib-2.47.1.2.1.1.2.0"
#define ENTITY_LOGICALCOMM_OID   "mib-2.47.1.2.1.1.4.0"



//
// Cisco CDP cache table
//
// "1.3.6.1.4.1.9.9.23.1.2.1.1.6.1" for getnext
#define CDP_DEVICEID_OID "enterprises.9.9.23.1.2.1.1.6"


//
// Device specifics
//
#define TEMP_OID         "1.3.6.1.4.1.9.9.13.1.3.1.3.1"
#define TEMP_STATE_OID   "1.3.6.1.4.1.9.9.13.1.3.1.6.1"
// cpu utilization
#define CPU_BUSYPER_OID  "1.3.6.1.4.1.9.2.1.56.0"
#define CPU_BUSY1_OID    "1.3.6.1.4.1.9.2.1.57.0"
#define CPU_BUSY5_OID    "1.3.6.1.4.1.9.2.1.58.0"


//
// BGP Peer Info OIDS
// 



//
// Cisco FWSM OIDS
// from CISCO-FWSM-MIB
//
#define FWSM_FAIL_STATUS     "1.3.6.1.4.1.9.9.147.1.2.1.1"


// 
// Cisco IOS Flash copy from CISCO-FLASH-MIB
//   
#define FLASHCOPY_CMD_OID           "enterprises.9.9.10.1.2.1.1.2"
#define FLASHCOPY_PROTO_OID         "enterprises.9.9.10.1.2.1.1.3"
#define FLASHCOPY_SERVER_OID        "enterprises.9.9.10.1.2.1.1.4"
#define FLASHCOPY_SRCNAME_OID       "enterprises.9.9.10.1.2.1.1.5"
#define FLASHCOPY_DSTNAME_OID       "enterprises.9.9.10.1.2.1.1.6"
#define FLASHCOPY_STATUS_OID        "enterprises.9.9.10.1.2.1.1.8"
#define FLASHCOPY_NOTIFY_OID        "enterprises.9.9.10.1.2.1.1.9"
#define FLASHCOPY_TIME_OID          "enterprises.9.9.10.1.2.1.1.10"
#define FLASHCOPY_ENTRY_OID         "enterprises.9.9.10.1.2.1.1.11"

#define FLASHCOPY_CMD_COPYTO        "2"
#define FLASHCOPY_CMD_COPYFROM      "3"
#define FLASHCOPY_PROTO_TFTP        "1"

#define FLASHCOPY_CMDENTRY_ACTIVATE "1"
#define FLASHCOPY_CMDENTRY_CREATEGO "4"
#define FLASHCOPY_CMDENTRY_DESTROY  "6"

#define FLASHCOPY_ERR_INPROGRESS    1
#define FLASHCOPY_ERR_SUCCESS       2
#define FLASHCOPY_ERR_OPERATION     3
#define FLASHCOPY_ERR_PROTOCOL      4
#define FLASHCOPY_ERR_SRCNAME       5
#define FLASHCOPY_ERR_DSTNAME       6
#define FLASHCOPY_ERR_SERVERADDR    7
#define FLASHCOPY_ERR_DEVBUSY       8
#define FLASHCOPY_ERR_DEVOPEN       9
#define FLASHCOPY_ERR_DEVERROR      10
#define FLASHCOPY_ERR_DEVFULL       12
#define FLASHCOPY_ERR_FILEOPEN      13
#define FLASHCOPY_ERR_FILETRANSFER  14
#define FLASHCOPY_ERR_FILECHECKSUM  15
#define FLASHCOPY_ERR_NOMEMORY      16
#define FLASHCOPY_ERR_UNKNOWN       17





#endif /*OIDS_H_*/
