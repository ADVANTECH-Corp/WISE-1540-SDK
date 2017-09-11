#ifndef __PARSER_H__
#define __PARSER_H__

#include 	<common/common.h>
#include    <datafmt/ipso/IPSO.h>
#include    <datafmt/ipso/SensorFormat.h>
//#include    <mcu/common/board.h>

#ifdef __cplusplus__
extern "C"
{
#endif

#include    <string.h>
#include    <stdlib.h>
#include    <errno.h>
#include    <stdio.h>

typedef enum 
{
    eParRetOk = 0,
    eParRetFail = 1
} eParRet;


typedef enum
{
    eURICmdOk,
    eURICmdSenInfoListAll,      //  GET /sen/info?action=list
    eURICmdSenInfoListFieldId,  //	GET /sen/info?action=list&field=id
    eURICmdSenInfoList,         //	GET /sen/info?action=list&id=0,1,2
    eURICmdQuerySenBySid,       //  GET /sen/info?action=query&sid=3303
    eURICmdQuerySenById,        //  GET /sen/info?action=query&id=0
    eURICmdObserve,             //  GET /sen/status?action=observe&id=all
    eURICmdCancel,              /// GET /sen/status?action=cancel&id=all
    eURICmdDevInfoList,         //  GET /dev/info?action=list
    eURICmdDevStatus,           //  GET /dev/status
    eURICmdSetDevName,          //  PUT /dev/info?n="Demo 1"
    eURICmdSetSenValueById,     //  PUT /sen/status?id=5&bv=1
    eURICmdSetSenValueByName,   //  PUT /sen/status?n=dout&bv=1
    eURICmdNetInfoList,         //  GET /net/info?action=list
    eURICmdNetStatus,           //  GET /net/status
    eURICmdReset,               //  PUT /dev/info?reset=1   
    eURICmdUnKnown
} eURICmd;

#define BUFF_SIZE_100   100
#define BUFF_SIZE_50    50
#define BUFF_SIZE_32    32
#define BUFF_SIZE_16    16 

// -----------------------------------------------------------
// Function Name: Parser_Init
// Description: Paser initialize interanl state and variables
//
// Pararmeters: None
//
// Return:  If successful return eParRetOk (0), else eParRetFail (-1)
//
eParRet Parser_Init(void); // Call Sensor_CheckIsReady to check Sensor is register!

// -----------------------------------------------------------
// Function Name: Parser_Decode
// Description: Decode the IPSO sensor format and set state machine
//
// Pararmeters:
//          1. pPacketRx: Pointer of Rx Coap Packet
//          2. PacketSize: Size of Packet, Must great to zero
//
// Return: URICmd, Ref eURICmd
//
eURICmd Parser_Decode(INT8* pPacketRx, INT32 PacketSize);

// -----------------------------------------------------------
// Function Name: Parser_Encode
// Description: Encode the IPSO sensor format and resend to sensor gateway
//
// Pararmeters:
//          1. URICmd:  URI Command and ref eURICmd
//          2. pPacketTx: Pointer of Tx Packet and Pointer can't be NULL
//          3. PacketSize: Size of Packet, Must great to zero
//
// Return:  Encode Packet Size, -1 means encode fail!
//
INT16 Parser_Encode(eURICmd URICmd,INT8* pPacketTx, INT16 PacketSize);

// -----------------------------------------------------------
// Function Name: Paser_FindKey
// Description: Find Key and return last position of char which not processed
//
// Pararmeters:
//          1. Cptr: Pointer of payload buffer
//          2. Key: Key
//
// Return: If successful return last position of char which not processed, else return NULL
//
INT8* Paser_FindKey(INT8 *Cptr, INT8 *Key);

// -----------------------------------------------------------
// Function Name: Paser_KeyVal
// Description: Find Key and return the Value ( Key=Value )
//
// Pararmeters:
//          1. Cptr: Pointer of payload buffer
//          2. Key: Key be Searched
//          3. MaxSize: The max size of buffer will be searched
//          4. Val: Pointer to buffer of Value
//          5. Size: Size of buffer of Value
//
// Return: If successful return last position of char which not processed, else return NULL
//
INT8* Paser_KeyVal(INT8 *Cptr, INT8*  Key, INT16 MaxSize, INT8 *Val, INT16 *Size);

// -----------------------------------------------------------
// Function Name: Paser_StrListId
// Description: Parse sensor list IDs and set ID
//
// Pararmeters:
//          1. SensorList: Sensor Id lists with string
//
// Return:  If successful return number of Sensor IDs, else return -1
//
INT16 Paser_StrListId(const INT8 *SensorList);

// -----------------------------------------------------------
// Function Name: Paser_SetSenId
// Description: Set ID number as be actived Sensor ID ( Max 16 sensor devices, 0~15 )
//
// Pararmeters:
//          1. SensorId: ID to be active Sensor ID
//
// Return: None
//
void Paser_SetSenId(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Paser_SetSenIdByString
// Description: Paseing Set Sensor Id string and set id bitmask
//
// Pararmeters:
//          1. SenIdString: Sensod Id String, For exapmle., id=0,1,2,4,5
//
// Return: None
//
void Paser_SetSenIdByString(INT8 *SenIdString);

// -----------------------------------------------------------
// Function Name: Paser_CheckSenId
// Description: Check Sensor ID is actived by selected SensorId
//
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return: If this Sensor Id is active return TRUE, else return FALSE
//
BOOL Paser_CheckSenId(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Parsr_itoa 
// Description: If OS no itoa function use it
//
// Pararmeters:
//          1. i: digit
//
// Return: return 0~9
//
INT8 *Parsr_itoa(INT32 i);

// -----------------------------------------------------------
// Function Name: Paser_GetCoapActionPtr
// Description: Get pointer of CoapAction Structure
//
// Pararmeters: None
//
// Return: reuturn pointer of CoapAction Structure
//
CoapAction_t *Paser_GetCoapActionPtr(void);

#if defined(TEST_PARSER)
/// -----------------------------------------------------------
// Function Name: Parser_Test
// Description: Self test with MCU Side
//
// Pararmeters:
//			1. PacketBuff: Pointer of Coap Packet
//			2. Legth: Buffer Legth
//			3. TestCase: Select Test Case
//			4. SensorId: Sensor Id 
//
// Return: reuturn length of Coap Packet
//
UINT8 Parser_Test(UINT8* PacketBuff, UINT8 Length, INT16 TestCase, INT16 SensorId, INT16 Sid);
#endif

#ifdef __cplusplus__
}
#endif

#endif // End of __PARSER_H__
