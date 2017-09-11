#ifndef __BOARD_H__
#define __BOARD_H__

#include "mbed.h"
#include <common/datatype.h>

//#define WISE_1540

//#if 0
//#if defined(WISE_1020)
//#include    <board/wise_1020/config.h>
//#elif defined(WISE_1021)
//#include    <board/wise_1021/config.h>
//#endif /* endif WISE_1020 */
//#endif 

#include    <board/wise_1540/config.h>
#include    <datafmt/ipso/SensorFormat.h>
#include    <datafmt/ipso/Parser.h>
#include    "mcu/stm32l443rc/pwr.h"
#include    "mcu/stm32l443rc/netdev.h"

#if defined(__cplusplus)
extern "C"
{
#endif


#define BOARD_NAME_MAX              16
#define SOFTWARE_VERSION_MAX        16
#define HARDWARE_VERSION_MAX        16
#define FLASH_BURN_MAGIC            0x5A5A

// ---------------- tagBoardConfig  -------------------------
typedef struct tagCapabilities
{
    UINT32  reset:1;        // All System Reset
    UINT32  pwrmgr:1;       // Power Management
    UINT32  reserved:14;    //   
} Capabilities_t, *Capabilities_tp;

// ---------------- tagBoardConfig  -------------------------
typedef struct tagBoardConfig
{
    INT16           magic;              // Magic Word, if equals to 0x5A5A menas first time self burn to Flash
    Capabilities_t  cap;                // Capability
    PowerConfig_t   pwr;                // Power Manager
    NetConfig_t     net;                // Net Config
    INT8 netdev_name[BOARD_NAME_MAX];   // dust, bluetooth, wifi
    INT8 hostname[BOARD_NAME_MAX];      // CO2,	Motion, Agriculture
    INT8 modelname[BOARD_NAME_MAX];     // S8, SHT21
    INT8 sw_version[SOFTWARE_VERSION_MAX];   
    INT8 hw_version[HARDWARE_VERSION_MAX];
    UINT8 padding[4];                   // Padding to multiple of 8 for flash programming
} BoardConfig_t, *BoardConfig_tp;

// -----------------------------------------------------------
// Function Name: Board_Init
// Description: Init Config Structures
// Pararmeters: None
//
// Return:  None
//
void  Board_Init(void);

// -----------------------------------------------------------
// Function Name: Board_SaveConfig
// Description: Save all config data to Flash Info ABCD (MSP430 MCU)
// Pararmeters: None
//
// Return:  If burn to flash successful return 0, else -1
//
INT16  Board_SaveConfig(void);

// -----------------------------------------------------------
// Function Name: Board_RestoreConfig
// Description: Restore Board config data from Flash Info ABCD (MSP430 MCU)
// Pararmeters: None
//
// Return:  None
//
void  Board_RestoreConfig(void);

// -----------------------------------------------------------
// Function Name: Board_FactoryDefault
// Description: Restore board config data with factory default values
// Pararmeters: None
//
// Return:  None
//
void  Board_FactoryDefault(void);

// -----------------------------------------------------------
// Function Name: Board_GetCfgPtr
// Description: Get pointer of board configuration structure
// Pararmeters: None
//
// Return:   return pointer of board configuration structure
//
BoardConfig_tp Board_GetCfgPtr(void);

// -----------------------------------------------------------
// Function Name: Board_GetFlashCfgPtr
// Description: Get pointer of board flash configuration structure
// Pararmeters: None
//
// Return:   If successful return pointer of board flash config, else return NULL
//
BoardConfig_tp Board_GetFlashCfgPtr(void);

// -----------------------------------------------------------
// Function Name: Board_GetHostName
// Description: Get Board Host Name
// Pararmeters: None
//
// Return:  If successful return Board Host Name, else NULL
//
INT8* Board_GetHostName(void);

// -----------------------------------------------------------
// Function Name: Board_SetHostName
// Description: Set Board Host Name
// Pararmeters:
//          1. HostName: Board Host with ASCII string
//
// Return:  If successful return string length, else -1
//
INT16  Board_SetHostName(const INT8 *HostName);

// -----------------------------------------------------------
// Function Name: Board_GetModelName
// Description: Get Board Model Name
// Pararmeters: None
//
// Return:  If successful return Board Model Name, else NULL
//
INT8* Board_GetModelName(void);

// -----------------------------------------------------------
// Function Name: Board_SetModelName
// Description: Set Board Model Name
// Pararmeters:
//          1. ModelName: Model name with ASCII string
//
// Return:  If successful return string length, else -1
//
INT16   Board_SetModelName(const INT8 *ModelName);

// -----------------------------------------------------------
// Function Name: Board_GetNetdevName
// Description: Get Netdev Name
// Pararmeters: None
//
// Return:  If successful return Netdev Name, else NULL
//
INT8* Board_GetNetdevName(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetdevName
// Description: Set Netdev Name
// Pararmeters:
//          1. NetdevName: Netdev name with ASCII string
//
// Return:  If successful return string length, else -1
//
INT16   Board_SetNetdevName(const INT8 *NetdevName);

// -----------------------------------------------------------
// Function Name: Board_GetSWVersion
// Description: Get Board Software Version
// Pararmeters: None
//
// Return:   return software version with ASCII string
//
INT8*  Board_GetSWVersion(void);

// -----------------------------------------------------------
// Function Name: Board_SetSWVersion
// Description: Set Board Software Version
// Pararmeters:
//          1. SWVersion: Set board software version
//
// Return:  If successful return string length, else -1
//
INT16	Board_SetSWVersion(const INT8 *SWVersion);

// -----------------------------------------------------------
// Function Name: Board_GetHWVersion
// Description: Get Board hardware Version
// Pararmeters: None
//
// Return:   return hardware version with ASCII string
//
INT8* Board_GetHWVersion(void);

// -----------------------------------------------------------
// Function Name: Board_SetHWVersion
// Description: Set Board Hardware Version
// Pararmeters:
//          1. HWVersion: Set board Hardware version
//
// Return:  If successful return string length, else -1
//
INT16  Board_SetHWVersion(const INT8 *HWVersion);

//**********************    Capabilities Config     *******************
// -----------------------------------------------------------
// Function Name: Board_GetCapReset
// Description: Get Capabilities of Reset
// Pararmeters: None
//
// Return:   return Capabilities of Reset ( 1: have reset function support, 0: not support reset function )
//
UINT32 Board_GetCapReset(void);

// -----------------------------------------------------------
// Function Name: Board_SetCapReset
// Description: Set Capabilities of Reset
// Pararmeters:
//          1. CapReset: Capabilities of Reset
//              ( 1: have reset function support, 0: not support reset function )
//
// Return:   If set Capabilities of Reset successful return 0, else -1
//
INT16 Board_SetCapReset(UINT32 CapReset);

// -----------------------------------------------------------
// Function Name: Board_GetCapPwr
// Description: Get Capabilities of Power
// Pararmeters: None
//
// Return:   return Capabilities of Power
//           ( 1: have power functions support, 0: not support power functions )
//
INT16 Board_GetCapPwr(void);

// -----------------------------------------------------------
// Function Name: Board_SetCapPwr
// Description: Set Capabilities of Power
// Pararmeters:
//          1. CapPwr: Capabilities of Power
//          (1: have power functions support, 0: not support power functions )
//
// Return:   If set Capabilities of Power successful return 0, else -1
//
INT16 Board_SetCapPwr(UINT32 CapPwr);

//**********************    Power Config   *******************
// -----------------------------------------------------------
// Function Name: Board_GetPwrMode
// Description: Get Power Mode, ref ePwrMode
// Pararmeters: None
//
// Return:   return Power Mode, ref ePwrMode
//
ePwrMode Board_GetPwrMode(void);

// -----------------------------------------------------------
// Function Name: Board_SetPwrMode
// Description: Set Power Mode
// Pararmeters:
//          1. PwrMode: Mode of Power, ref ePwrMode
//
// Return:   If set Power Mode successful return 0, else -1
//
INT16 Board_SetPwrMode(ePwrMode PwrMode);

//**********************    NET Config     *******************
// -----------------------------------------------------------
// Function Name: Board_GetNetIfType
// Description: Get Net Type of Interface
// Pararmeters: None
//
// Return:   return Net Type of Interface, ref eNetIf
//
eNetIf Board_GetNetIfType(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetIfType
// Description: Set Net Type of Interface
// Pararmeters:
//          1. NetIf: Net Type of Interface
//
// Return:   If set Net Interace Type successful return 0, else -1
//
INT16 Board_SetNetIfType(eNetIf NetIf);

// -----------------------------------------------------------
// Function Name: Board_GetNetNodeType
// Description: Get Net Type of Node
// Pararmeters: None
//
// Return:   return Net Type of Node, ref eNetNodeType
//
eNetNodeType Board_GetNetNodeType(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetNodeType
// Description: Set Net Type of Node
// Pararmeters:
//          1. NetNodeType: Net Type of Node
//
// Return:   If set Net Node Type successful return 0, else -1
//
INT16 Board_SetNetNodeType(eNetNodeType NetNodeType);

// -----------------------------------------------------------
// Function Name: Board_GetNetPort
// Description: Get Net Port Number
// Pararmeters: None
//
// Return:   return Net Port Number
//
UINT32 Board_GetNetPort(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetPort
// Description: Set Net Port Number
// Pararmeters:
//          1. Port: Port Number
//
// Return:   If Set Net Port Number successful return 0, else -1
//
INT16 Board_SetNetPort(UINT32 Port);

// -----------------------------------------------------------
// Function Name: Board_GetNetIP
// Description: Get Net IP Address
// Pararmeters: None
//
// Return:   return IP Address with Ascii string
//
INT8* Board_GetNetIP(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetIP
// Description: Set Net IP Address
// Pararmeters:
//          1. IP: IP Address with Ascii string
//
// Return:   If Set IP Address successful return length of string, else -1
//
INT16 Board_SetNetIP(const INT8 *IP);

// -----------------------------------------------------------
// Function Name: Board_GetNetGateway
// Description: Get Net Gateway Address
// Pararmeters: None
//
// Return:   return Gateway with Ascii string
//
INT8* Board_GetNetGateway(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetGateway
// Description: Set Net Gateway Address
// Pararmeters:
//          1. Gateway: Gateway Address with Ascii string
//
// Return:   If Set Net Gateway Address successful return length of string, else -1
//
INT16 Board_SetNetGateway(const INT8 *Gateway);

// -----------------------------------------------------------
// Function Name: Board_GetNetUsername
// Description: Get Net Account Username
// Pararmeters: None
//
// Return:   return Account Username with Ascii string
//
INT8* Board_GetNetUsername(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetUsername
// Description: Set Net Account Username
// Pararmeters:
//          1. Username: Account Username with Ascii string
//
// Return:   If Set Net Account Username successful return length of string, else -1
//
INT16 Board_SetNetUsername(const INT8 *Username);

// -----------------------------------------------------------
// Function Name: Board_GetNetPassword
// Description: Get Net Account Password
// Pararmeters: None
//
// Return:   return Account Password with Ascii string
//
INT8* Board_GetNetPassword(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetPassword
// Description: Set Net Account Password
// Pararmeters:
//          1. Password: Account Password with Ascii string
//
// Return:   If Set Net Account Password successful return length of string, else -1
//
INT16 Board_SetNetPassword(const INT8 *Password);

// -----------------------------------------------------------
// Function Name: Board_GetNetMac
// Description: Get Net Mac Address
// Pararmeters: None
//
// Return:   return Mac Address with Ascii string
//
INT8* Board_GetNetMac(void);

// -----------------------------------------------------------
// Function Name: Board_SetNetMac
// Description: Set Net Mac Address
// Pararmeters:
//          1. Mac: Mac Address with Ascii string
//
// Return:   If Set Mac AcAddress successful return length of string, else -1
//
INT16 Board_SetNetMac(const INT8 *Mac);

#if defined(__cplusplus)
}
#endif

#endif // End of __BOARD_H__
