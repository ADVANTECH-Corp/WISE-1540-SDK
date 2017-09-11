#ifndef __SENSOR_FORMAT_H__
#define __SENSOR_FORMAT_H__

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

//	---------- enum Data Type--------
typedef enum
{
    eDataTypeUnKnown = 0,
    eDataTypeU8 = 1,
    eDataTypeS8 = 2,
    eDataTypeU16 = 3,
    eDataTypeS16 = 4,
    eDataTypeU32 = 5,
    eDataTypeS32 = 6,
    eDataTypeFloat = 7,
    eDataTypeBoolean = 8
} eDataType;

//	---------- enum Sensor Unit -------------
typedef enum
{
    SenUnitNo = 0,
    SenUnitVolgate = 1, //  V ( Voltage )
    SenUnitCurrent = 2,	//  A ( Amper ) 
    SenUnitCel = 3,	    //  ºC (Celsius) 
    SenUnitPPM = 4,		//  ppm
    SenUnitG = 5,		//  CGS Gaussian units
    SenUnitg = 6,		//  Gravity units
    SenUnitlx = 7, 		//  Light Sensor, Flux
    SenUnitgM = 8,		//  MKS M ( Length meter )
    SenUnitgCM = 9,		//  CGS m ( Length meter )
    SenUnitDbm = 7      //  Decibel milliwatts
} eSenUnit;

// -------- enum Sensor Type ----------------
typedef enum
{
    eTypeUnknown = 0,
    eTypeTemperature = 3303,
    eTypeHumidity = 3304,
    eTypeMagnetic = 3314,
    eTypeAccelerometer = 3313,
    eTypeLightSensor = 3301,
    eTypeDigitalOutput = 3201,	
    eTypeDigitalInput = 3200,
    eTypeCO2 = 9001,
    eTypeCO = 9002,
    eTypeUltrasonic = 9200,
    eTypeMax = 32767
} eSenType;  // Value the same as ISPO Smart Id according its type

// -------- enum Sensor Functions Return Code ---------
typedef enum
{
    eSenRetFail = -1,
    eSenRetOk = 0,
    eSenRetAddDeviceFail = 1,
    eSenRetDelDeviceFail = 2,
    eSenRetUnKnown = 3
} eSenRet;

// ------------ Sensor Host Name and Model Name  --------------
#define SENSOR_HOSTNAME_MAX     16
#define SENSOR_MODELNAME_MAX    16

// ------------ General purpose Sensor Structure  -------------
typedef struct tagSenStrcut
{
   UINT16	sid;    // Sensor Smart Object ID
   UINT16   rid;    // Resource ID
   float	max;    // Max range
   float  	min;    // Min range
   float	x;      // X-Axis
   float	y;      // Y-Axis
   float  	z;      // Z-Axis
   float  	dir;    // Direction
   eSenUnit unit;   
   eDataType DataType;
   union uData
   {
     BOOL   bVal;
     float  fVal;
     INT32  iVal;
   } Data;

   INT8 hostname[SENSOR_HOSTNAME_MAX];
   INT8	modelname[SENSOR_MODELNAME_MAX];
} SenStrcut_t, *SenStrcut_tp;

// ------------ Sensor Structure List ------------------
typedef struct tagSensorStructList
{
    eSenType SenType;              	// Ref eSensorType
    SenStrcut_t SensorStruct;     	// Pointer of Sensor Structure
} SensorStructList_t, * SensorStructList_tp;

#define SENSOR_STRUCTURE_LIST_MAX   8

#define SENSOR_NAME_MAX 16

typedef struct tagSensorName
{
    UINT16 sid; // Smart Object Id  
    INT8  sensorName[SENSOR_NAME_MAX];
} SensorName_t, *SensorName_tp;

// -----------------------------------------------------
#define STANDARD_UNIT_MAX           16
#define STANDARD_STANDARD_MAX       16
#define STANDARD_RESOURCETYPE_MAX   16
#define STANDARD_ACCESSMODE_MAX     16
#define STANDARD_DATATYPE_MAX       16
#define STANDARD_ARRAY_MAX     		12

typedef struct standardStrcut
{
   UINT16 sid; // Smart Object Id
   INT8 unit[STANDARD_UNIT_MAX];
   INT8 standard[STANDARD_STANDARD_MAX];
   INT8 resourceType[STANDARD_RESOURCETYPE_MAX];
   INT8 accessMode[STANDARD_ACCESSMODE_MAX];
   INT8 dataType[STANDARD_ACCESSMODE_MAX];
} Standard_t, *Standard_tp;



// -----------------------------------------------------
// Function Name: Sensor_DeviceInit
// Description: Iinitial sensor structure
// Pararmeters:  None
//
// Return:  Ref eSenRet
//
eSenRet Sensor_DeviceInit(void);

// -----------------------------------------------------
// Function Name: Sensor_AddDevice
// Description: Add sensor devices to sensor structure lists, and return Sensor Id
//
// Pararmeters:  1. Sid: Sensor Smart Id or enum eSenType
//				2. StructSensorPtr:  Pointer of Sensor' structure
//
// Return:  If successsful the return Sensor ID, else -1
//
INT16 Sensor_AddDevice(INT16 Sid, SenStrcut_t *StructSensorPtr);

// -----------------------------------------------------
// Function Name: Sensor_DelDevice
// Description: Remove Sensor from Sensor Array
// Pararmeters:  1. SensorId: Sensor ID
//
// Return:  Ref enum eSenRet, Sensor Functions Return Code
//
eSenRet Sensor_DelDevice(INT16 SensorId);

// -----------------------------------------------------
// Function Name: Sensor_GetStructPtrById
// Description: Get pointer of Sensor structure by Sensor Id from Sensor Structure Lists 
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Pointer of Sensor structure, else return NULL
//
SenStrcut_t* Sensor_GetStructPtrById(INT16 SensorId);

// -----------------------------------------------------
// Function Name: Sensor_GetStructPtrByHostname
// Description: Get pointer of Sensor structure by Sensor Hostname from Sensor Structure Lists
// Pararmeters:
//          1. Hostname: Sensor Hostname
//
// Return:  If successful return Pointer of Sensor structure, else return NULL
//
SenStrcut_t* Sensor_GetStructPtrByHostname(INT8 *Hostname);

// -----------------------------------------------------
// Function Name: Sensor_GetData
// Description: Get sensor data from Sensor Structure
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. StructSensorPtr: Pointer of Sensor structure which selectd by Sensor ID
//
// Return:  Ref enum eSenRet, Sensor Functions Return Code
//
eSenRet Sensor_GetData(INT16 SensorId, SenStrcut_t *StructSensorPtr);

// -----------------------------------------------------------
// Function Name: Sensor_SetData
// Description: Set sensor value to sensor structure
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. StructSensorPtr: Set sensor value by this structure
//
// Return:  If successful return eSenRetOk (0), else eSenRetFail (-1)
//
eSenRet Sensor_SetData(INT16 SensorId, SenStrcut_t *StructSensorPtr);

// -----------------------------------------------------------
// Function Name: Sensor_SetHostName
// Description: Set Sensor Host Name
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. HostName: Host name with ASCII string
//
// Return:  If successful return length of string, else -1
//
INT16  Sensor_SetHostName(INT16 SensorId, const INT8 *HostName);

// -----------------------------------------------------------
// Function Name: Sensor_GetHostName
// Description: Get Sensor Host Name by selected Sensor ID
// Pararmeters:
//           1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Host Name with Ascii string, else NULL
//
INT8* Sensor_GetHostName(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Sensor_SetModelName
// Description: Set Sensor Model Name by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. ModelName: Sensor Model Name with ASCII string
//
// Return:  If successful return length of string, else -1
//
INT16  Sensor_SetModelName(INT16 SensorId, const INT8 *ModelName);

// -----------------------------------------------------------
// Function Name: Sensor_GetModelName
// Description: Get Sensor Model Name
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Model Name, else NULL
INT8* Sensor_GetModelName(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Sensor_GetSensorName
// Description: Get Sensor Name by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Name with Ascii string, else return NULL
//
INT8* Sensor_GetSensorName(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Sensor_GetSmid
// Description: Get Sensor Smart Object Id by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Smart Object Id , else -1
//
INT16 Sensor_GetSmid(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Sensor_SetSmid
// Description: Set Sensor Smart Object ID
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. Sid: Set Smart Object ID 
//
// Return:  If successful return 0, else -1
//
INT16 Sensor_SetSmid(INT16 SensorId, INT16 Sid);

// -----------------------------------------------------------
// Function Name: Sensor_GetType
// Description: Get Sensor Type by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Type, else eTypeUnknown=-1
// 
eSenType Sensor_GetType(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Sensor_GetDataType
// Description: Get Sensor Data Type by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Data Type, else eDataTypeUnKnown=0
//
eDataType Sensor_GetDataType(INT16 SensorId);

// -----------------------------------------------------------
// Function Name: Sensor_GetCount
// Description: Get Sensor Count, that have add to system
// Pararmeters: None
//
// Return:  return numbers of Sensor
//
INT16 Sensor_GetCount(void);

// -----------------------------------------------------------
// Function Name: Sensor_ClearCount
// Description: Clean Sensor Count
// Pararmeters: None
//
// Return:  None
//
void Sensor_ClearCount(void);

// -----------------------------------------------------------
// Function Name: Sensor_GetSmidBySenType
// Description: Get Sensor Smart Object ID by Sensor Type
// Pararmeters:
//          1. eSensorType: Sensor Type
//
// Return:  If successful return IPSO Smart Object ID, else eTypeUnknown or -1
//
INT16 Sensor_GetSmidBySenType(eSenType eSensorType);


// -----------------------------------------------------------
// Function Name: Sensor_GetIdByHostname
// Description: Get Sensor ID by search HostName
// Pararmeters:
//          1. Hostname: Sensor Host Name
//
// Return:  If successful return Sensor ID, else -1
//
INT16 Sensor_GetIdByHostname(INT8 *Hostname);

// -----------------------------------------------------------
// Function Name: Sensor_GetIdByModelname
// Description: Get Sensor ID by search Modelname
// Pararmeters:
//          1. Modelname: Sensor Model Name
//
// Return:  If successful return Sensor ID, else -1
//
INT16 Sensor_GetIdByModelname(INT8 *Modelname);

// -----------------------------------------------------------
// Function Name: Sensor_GetUnit
// Description: Get Mote Unit string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object ID
//
// Return:  If successful return IPSO unit string, else -1
//
INT8* Sensor_GetUnit(INT16 Sid);

// -----------------------------------------------------------
// Function Name: Sensor_GetStandard
// Description: Get Mote Standard string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object Id
//
// Return:  If successful return IPSO standard string, else -1
//
INT8* Sensor_GetStandard(INT16 Sid);

// -----------------------------------------------------------
// Function Name: Sensor_GetResourceType
// Description: Get Mote Resource Type string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object Id
//
// Return:  If successful return IPSO resource type string, else -1
//
INT8* Sensor_GetResourceType(INT16 Sid);

// -----------------------------------------------------------
// Function Name: Sensor_GetAccessMode
// Description: Get Mote Access Mode string by Smart Object ID
// Pararmeters:
//          1. sid: Smart Object Id
//
// Return:  If successful return IPSO Access Mode string, else -1
//
INT8* Sensor_GetAccessMode(INT16 Sid);

// -----------------------------------------------------------
// Function Name: Sensor_GetDatatype
// Description: Get Sensor Data Type string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object Id
//
// Return:  If successful return IPSO Data Type string, else -1
//
INT8* Sensor_GetDataTypeBySid(INT16 Sid);

#ifdef TEST_EMULATOR
void  Sensor_Test(void);
#endif

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // End of __SENSOR_FORMAT_H__
