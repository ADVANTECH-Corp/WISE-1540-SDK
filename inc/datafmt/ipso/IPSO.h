#ifndef __IPSO_H__
#define __IPSO_H__

#include  <common/datatype.h>

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

//+--------------------+-----------+--------------+
//| Function Set       | Root Path | Resouce Type |
//+--------------------+-----------+--------------+
//| Device             | /dev      | ipso.dev     |
//| General Purpose IO | /gpio     | ipso.gpio    |
//| Power              | /pwr      | ipso.pwr     |
//| Load Control       | /load     | ipso.load    |
//| Sensors            | /sen      | ipso.sen     |
//| Light Control      | /lt       | ipso.lt      |
//| Message            | /msg      | ipso.msg     |
//| Location           | /loc      | ipso.loc     |
//| Configuration      | /cfg      | ipso.cfg     |
//+--------------------+-----------+--------------+

//=========================================
//          Adv MUC Packet Type
//========================================
//				 8 Bytes               1 ~ 70 Bytes
//|---------------|----------------------------
//|  CoAP Header  |      PLYLOAD              |
//|---------------|----------------------------
//  0                 1                     2                           3
//  0 1 2  3 4 5  6 7 8 9  0 1 2 3 4  5 6 7 8 9 0 1  2 3 4  5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+
//  |Ver| T |   TKL   |      Code           |          Message ID        |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+
//  |                        Length                                      |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+
//
#define COAP_PAYLOAD_SIZE_MAX 70
#define COAP_PACKET_SIZE_MAX ( 8 + COAP_PAYLOAD_SIZE_MAX )
//  Total Header Size : 8 Bytes
//  Fixed Value
//	Ver:               2 bits        1
//	Type:              2 bits        x
//	TKL: Token Length: 4 bits        0
//	Code:              8 bits        x
//	Message ID:       16 bits        x
//	Length:           32 bits        x
// ------------ COAP Packet Structure  --------------
typedef struct tagCoapHeader
{
   UINT32 Ver: 2;       //  Version
   UINT32 Type: 2;      //  Transaction Type
   UINT32 Tkl: 4;       //
   UINT32 Code: 8;      //  Code   MSB [3Bits|5Bits] LSB
   UINT32 Msgid: 16;
   UINT32 Length;
} CoapHeader_t;

//  ---------------------------------------------------------------
//  Version (Ver): 2-bit unsigned integer. Indicates the CoAP version
//  number. Implementations of this specification MUST set this field
//  to 1 (01 binary). Other values are reserved for future versions.
//  Messages with unknown version numbers MUST be silently ignored.
//  ---------------------------------------------------------------
//  Type (T): 2-bit unsigned integer. Indicates if this message is of
//  type Confirmable (0), Non-confirmable (1), Acknowledgement (2), or
//  Reset (3). The semantics of these message types are defined in
//  ---------------------------------------------------------------
//  Code: 8-bit unsigned integer, split into a 3-bit class (most
//  significant bits) and a 5-bit detail (least significant bits),
//  Code Value: class * 32 + details

typedef struct tagCoapPacket
{
   CoapHeader_t Header;
   INT8 Payload[COAP_PAYLOAD_SIZE_MAX];
} CoapPacket_t;

//				Ver: 1
//	T: Direction: GW -> MCU (1)
//	==========================================
//				   CoAP Code
//  ==========================================

//	Code
//	Request
//	* GET(0.01)                 -> 1
//	* PUT(0.03)                 -> 3
//	Response
//	* Change (2.04)             -> 68
//	* Content(2.05)             -> 69
//	* Bad Request (4.00)        -> 128
//	* Unauthorized (4.01)       -> 129
//	* Not Found (4.04)          -> 132
//	* Method Not Allowed (4.05) -> 133
//  * Not Implemented (5.01)    -> 161

// ------------ enum CoAP Code  --------
typedef enum
{
   eCoapCodeUnKnown = 0,
   // Request
   eCoapCodeGet = 1,
   eCoapCodePut = 3,
   // Response
   eCoapCodeChage = 68,
   eCoapCodeContent = 69,
   eCoapCodeBadRequest = 128,
   eCoapCodeUnauthorized = 129,
   eCoapCodeMethodNotAllowed = 132,
   eCoapCodeCNotFound =133,
   eCoapCodeNotImplemented =161
} eCoapCode;

// ------------ enum Coap Type  --------
typedef enum
{
    eCoapTypeConfirmable = 0,       // Need ACK
    eCoapTypeNonConfirmable=1,      // Don't need ACK
    eCoapTypeAck = 2,               // Specific Confirmable message (identified by its Transaction ID)
    eCoapTypeReset = 3,             //
    eCoapTypeUnKnown = 4
} eCoapType;

// T: 2-bit unsigned integer Transaction Type field.
// Indicates if this message is Confirmable (0), Non-Confirmable (1), Acknowledgment (2) or Reset (3).


// Option Count (OC): 4-bit unsigned integer. Indicates the number of options
// after the header (0-14).
// If set to 0, there are no options and the payload (if any) immediately follows the header.
// If set to 15, then an end-of-options marker is used to indicate the end of options and
// the start of the payload. The format of options is defined below.

// ------------ enum IPSO Function Type  ---------------------------------------
typedef enum            //   | Function Set         | Root Path | Resouce Type |
{                       //   |----------------------+---------------------------
    eIPSOTypeDevice,    //   | Device               | /dev      | ipso.dev     |
    eIPSOTypeGPIO,      //   | General Purpose IO   | /gpio 	| ipso.gpio    |
    eIPSOTypePower,     //   | Power                | /pwr      | ipso.pwr     |
    eIPSOTypeLoad,      //   | Load Control         | /load     | ipso.load    |
    eIPSOTypeSensor,    //   | Sensors              | /sen      | ipso.sen     |
    IPSOTypeLight,      //   | Light Control        | /lt       | ipso.lt      |
    IPSOTypeMessage,    //   | Message              | /msg      | ipso.msg     |
    IPSOTypeLocation,   //   | Location             | /loc      | ipso.loc     |
    IPSOTypeConfig,     //   | Configuration        | /cfg      | ipso.cfg     |
    IPSOTypeUnKnown_Func// -----------------------------------------------------
} eIPSOType;


//  =====================================
//	             IPSO
//  =====================================
// Restful API Rule:  /{Object ID}/{Object Instance}/{Resource ID}
//
//	---------- IPSO Smart Object ID -----
//      Temperature    3303
//      Humidity       3304
//      Magnetic       3314
//		Accelerometer  3313
//		Light Sensor   3301
//		Digital Output 3201
//		Digital Input  3200
//		CO2            9001
//		CO             9002
//		Ultrasonic     9200

typedef enum
{
    eSMOIdTemperature = 3303,
    eSMOIdHumidity = 3304,
    eSMOIdMagnetic = 3314,
    eSMOIdAccelerometer = 3313,
    eSMOIdLightSensor = 3301,
    eSMOIdDO = 3201,
    eSMOIdDI = 3200,
    eSMOIdCO2 = 9001,
    eSMOIdCO = 9002,
    eSMOIdUltrasonic = 9200
} eSMOId;

#define IPSO_SMO_ID_TEMPERATURE      3303
#define IPSO_SMO_ID_HUMIDITY      	 3304
#define IPSO_SMO_ID_MAGNETIC         3314
#define IPSO_SMO_ID_ACCELEROMETER    3313
#define IPSO_SMO_ID_LightSensor      3301
#define IPSO_SMO_ID_DO               3201
#define IPSO_SMO_ID_DI               3200
#define IPSO_SMO_ID_CO2              9001
#define IPSO_SMO_ID_CO               9002
#define IPSO_SMO_ID_Ultrasonic       9200


//	---------- IPSO Smart Resource ID -----
//    Digital Input State     5500
//    Digital Output State    5550
//    Min Range Value         5603
//    Max Range Value         5604
//    Sensor Value            5700
//    Sensor Units            5701
//    X Value                 5702
//    Y Value                 5703
//    Z Value                 5704
//    Compass Direction       5705

typedef enum
{
    eSMRIdDIState = 5500,
    eSMRIdDOState = 5550,
    eSMRIdMinVal = 5603,
    eSMRIdMaxVal = 5604,
    eSMRIdSenVal = 5700,
    eSMRIdSenUnits = 5701,
    eSMRIdXVal = 5702,
    eSMRIdYVal = 5703,
    eSMRIdZVal= 5704,
    eSMRIdCompassDir = 5705
} eSMRId;

#define IPSO_SMR_ID_DI_STATE    5500
#define IPSO_SMR_ID_DO_STATE    5550
#define IPSO_SMR_ID_MIN_VAL     5603
#define IPSO_SMR_ID_MAX_VA      5604
#define IPSO_SMR_ID_SEN_VAL     5700
#define IPSO_SMR_ID_SEN_UNIT    5701
#define IPSO_SMR_ID_AXIS_X_VAL  5702
#define IPSO_SMR_ID_AXIS_Y_VAL  5703
#define IPSO_SMR_ID_AXIS_Z_VAL  5704
#define IPSO_SMR_ID_COMPASS_DIR 5705

#define IPSO_SEN_UINT_Volgate   "v"
#define IPSO_SEN_UINT_Current   "a"
#define IPSO_SEN_UINT_Cel       "Cel"
#define IPSO_SEN_UINT_PPM       "ppm"
#define IPSO_SEN_UINT_G         "G"
#define IPSO_SEN_UINT_g         "g"
#define IPSO_SEN_UINT_Dbm       "dbm"
#define IPSO_SEN_UINT_RH        "RH"

#define COAP_ACTION_NAME_MAX    32
#define COAP_ACTION_ID_MAX      16

// ----------------------------------------
typedef struct tagCoapAction
{
   INT16  id;      	// Sensor Id
   INT16  sid;     	// Smart Object Id
   INT16  rid;     	// Resource Object Id
   struct tagReg
   {
        UINT32 dev:1;         //  0   /dev       
        UINT32 gpio:1;        //  1   /gpio
        UINT32 pwr:1;         //  2   /pwr
        UINT32 load:1;        //  3   /load
        UINT32 sen:1;         //  4   /sen
        UINT32 lt:1;          //  5   /lt
        UINT32 msg:1;         //  6   /msg
        UINT32 loc:1;         //  7   /loc
        UINT32 cfg:1;         //  8   /cfg
        UINT32 net:1;         //  9   /net
        UINT32 info: 1;       //  10  /info      : Information of Sensors
        UINT32 status: 1;     //  11  /status    : Status of Sensors
        UINT32 query: 1;      //  12  /query     : Query of Sensors
        UINT32 list: 1;       //  13  /action = list : List detail information of this resource    /sen/info
        UINT32 observe: 1;    //  14  /observe : Observe sensor's status                           /sen/status
        UINT32 cancel: 1;     //  15  /cancel  : Cancel observe sensor's status                    /sen/status
        UINT32 field: 1;      //  16  /filed=id      
        UINT32 health: 1;     //  17  /filed health  /net/status
        UINT32 bv: 1;         //  18  Boolean Value
        UINT32 iv: 1;         //  19  Integer Value
        UINT32 dv: 1;         //  20  Decimal Value
        UINT32 fv: 1;         //  21  float Value
        UINT32 name: 1;       //  22  /Host Name or Model Name, observe by name
        UINT32 all: 1;        //  23  /id = all, when idall=1, observe by id = all
        UINT32 reset: 1;      //  24  /reset system
        UINT32 sw: 1;         //  25  sw version
        UINT32 hw: 1;         //  26  hw version
        UINT32 id: 1;         //  27  Sensor Id
        UINT32 sid:  1;       //  28  smart object Id
        UINT32 rid:  1;       //  29  resource Id
        UINT32 reserved: 2;   //  30~31
   } reg;
   struct tagSensorID
   {
       UINT32 id0:  1;        //  0
       UINT32 id1:  1;        //  1
       UINT32 id2:  1;        //  2
       UINT32 id3:  1;        //  3
       UINT32 id4:  1;        //  4
       UINT32 id5:  1;        //  5
       UINT32 id6:  1;        //  6
       UINT32 id7:  1;        //  7
       UINT32 id8:  1;        //  8
       UINT32 id9:  1;        //  9
       UINT32 id10: 1;        //  10
       UINT32 id11: 1;        //  11
       UINT32 id12: 1;        //  12
       UINT32 id13: 1;        //  13
       UINT32 id14: 1;        //  14
       UINT32 id15: 1;        //  15
       UINT32 reserved: 16;   //  16~31
   } sen;
   INT8 name[COAP_ACTION_NAME_MAX]; // Hostname or Modelname
   union coapData
   {
     BOOL   bVal;
     float  fVal;
     INT32  iVal;
   } Data;
} CoapAction_t, *CoapAction_tp;

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // End of __IPSO_H__
