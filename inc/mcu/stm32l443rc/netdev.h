#ifndef __NETDEV_H__
#define __NETDEV_H__

#include    <common/datatype.h>

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    eNetIfNoDevice = 0,
	eNetIfWire = 1,
    eNetIfBlueTooth = 2,
    eNetIfWifi = 3,
    eNetIfZigBee = 4,
	eNetIfUWB = 5,
	eNetIfSub1G = 6,
	eNetIfDust = 7,
	eNetIfUnKnown = 8
} eNetIf;

typedef enum
{
    eNetNodeNo = 0,
	eNetNodeBus = 1,
    eNetNodeStar = 2,
    eNetNodeMesh = 3,    
	eNetNodeUnKnown = 4
} eNetNodeType;

#define NET_IP_MAX               16
#define NET_GATEWAY_MAX          16
#define NET_USERNAME_MAX         16
#define NET_PASSWORD_MAX         10
#define NET_MAC_MAX              20

#define NET_DEFAULT_IP           "192.168.0.2"
#define NET_DEFAULT_PORT         5058
#define NET_DEFAULT_GATEWAY      "192.168.0.1"  // Agent
#define NET_DEFAULT_USERNAME     "adv"
#define NET_DEFAULT_PASSWORD     "123456"
#define NET_DEFAULT_MAC          "01:00:00:0A:0B:0C"

// ---------------- tagConnectConfig  -------------------------
typedef struct tagNetConfig
{
    eNetIf  net_if;                 // Type of Net Device
    eNetNodeType node_type;         // Type of Net Node
    UINT32 port;                    // Port
    INT8 ip[NET_IP_MAX];            // IP
    INT8 gateway[NET_GATEWAY_MAX];  // Gateway
    INT8 username[NET_USERNAME_MAX];// Username
    INT8 password[NET_PASSWORD_MAX];// Password
    INT8 mac[NET_MAC_MAX];  		// Mac address
} NetConfig_t, *NetConfig_tp;

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif	// End of __NETDEV_H__
