#include    <datafmt/ipso/SensorFormat.h>
#include	<string.h>

#if defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=2
#endif

SensorStructList_t m_SensorStructList[SENSOR_STRUCTURE_LIST_MAX]; // Save MCU board all sensor devices
static int m_SensorCount = 0; // Total sensors in MCU Board
static int m_SensorIndex = 0; // Current seleced Sensor for m_SensorStructList

//const INT8* m_SensorName[]=
const SensorName_t m_SensorName[]=
{
  { 0, "Unknown"},
  { 3303, "Temperature"},
  { 3304, "Humidity"},
  { 3314, "Magnetic"},
  { 3313, "Accelerometer"},
  { 3301, "LightSensor"},
  { 3201, "DigitalOutput"},
  { 3200, "DigitalInput"},
  { 9001, "CO2"},
  { 9002, "CO"},
  { 9200, "Ultrasonic"},
  //{ -1, "Last"}
  { 10000, "Last"}
};

const Standard_t m_Standards[]=
{
    // Smart Object Id,  unit, standard, resourceType, accessMode, dataType
    //{0, "", "", "", "n", ""},
    {3200, "", "ipso", "gpio.din", "r", "b"},
    {3201, "", "ipso", "gpio.dout", "rw", "b"},
    {3301, "lx", "ipso", "ucum.lx", "r", "d"},
    {3303, "Cel", "ipso", "ucum.Cel", "r", "d"},
    {3304, "%", "ipso", "ucum.%", "r", "d"},
    {3313, "g", "ipso", "ucum.g", "r", "e"},
    {3314, "G", "ipso", "ucum.G", "r", "e"},
    {9001, "ppm", "ipso", "ucum.ppm", "r", "d"},
    {9002, "ppm", "ipso", "ucum.ppm", "r", "d"},
    {9200, "cm", "ipso", "ucum.cm", "r", "d"},
    {0, "", "", "", "n", ""}
};
 
// -----------------------------------------------------
// Function Name: Sensor_DeviceInit
// Description: Iinitial sensor structure
// Pararmeters:  None
//
// Return:  Ref eSenRet
//
eSenRet Sensor_DeviceInit(void)
{
    eSenRet RetCode = eSenRetOk;

    m_SensorCount = 0;
    m_SensorIndex = 0;

    memset(m_SensorStructList, 0, sizeof(SensorStructList_t)*SENSOR_STRUCTURE_LIST_MAX);        

#ifdef TEST_EMULATOR
    Sensor_Test();
#endif

    return RetCode;
}


// -----------------------------------------------------
// Function Name: Sensor_AddDevice
// Description: Add sensor devices to sensor structure lists, and return Sensor Id
//
// Pararmeters:  1. Sid: Sensor Smart Id or enum eSenType
//				2. StructSensorPtr:  Pointer of Sensor' structure
//
// Return:  If successsful the return Sensor ID, else -1
//
INT16 Sensor_AddDevice(INT16 Sid, SenStrcut_t *StructSensorPtr)
{
    INT16 SensorId = 0;     // Full or Failed!

    if(StructSensorPtr==NULL || m_SensorCount>SENSOR_STRUCTURE_LIST_MAX)
         return SensorId;

    SensorId = m_SensorCount;
    m_SensorStructList[m_SensorCount].SenType = (eSenType) Sid;
    m_SensorStructList[m_SensorCount].SensorStruct = *StructSensorPtr;
    m_SensorStructList[m_SensorCount].SensorStruct.sid = Sid;

    m_SensorCount++;

    return SensorId;
}

// -----------------------------------------------------
// Function Name: Sensor_DelDevice
// Description: Remove Sensor from Sensor Array
// Pararmeters:  1. SensorId: Sensor ID
//
// Return:  Ref enum eSenRet, Sensor Functions Return Code
//
eSenRet Sensor_DelDevice(INT16 SensorId)
{
    eSenRet RetCode = eSenRetOk;


    return RetCode;
}

// -----------------------------------------------------
// Function Name: Sensor_GetStructPtrById
// Description: Get pointer of Sensor structure by Sensor Id from Sensor Structure Lists
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Pointer of Sensor structure, else return NULL
//
SenStrcut_t* Sensor_GetStructPtrById(INT16 SensorId)
{
    SenStrcut_tp StructSensorPtr;
    
    StructSensorPtr = NULL;

    if(SensorId>SENSOR_STRUCTURE_LIST_MAX)
        StructSensorPtr = NULL;
    else
        StructSensorPtr = (SenStrcut_tp) &(m_SensorStructList[SensorId].SensorStruct);

    return StructSensorPtr;
}

// -----------------------------------------------------
// Function Name: Sensor_GetStructPtrByHostname
// Description: Get pointer of Sensor structure by Sensor Hostname from Sensor Structure Lists 
// Pararmeters:
//          1. Hostname: Sensor Hostname
//
// Return:  if successful return Pointer of Sensor structure, else return NULL
//
SenStrcut_tp Sensor_GetStructPtrByHostname(INT8 *Hostname)
{
    SenStrcut_tp StructSensorPtr;
    INT16 i;
    StructSensorPtr = NULL;

    for(i=0;i<m_SensorCount;i++)
    {
        if(strcmp((char*)m_SensorStructList[i].SensorStruct.hostname, (char*)Hostname)==0)
        {
            StructSensorPtr = (SenStrcut_tp) &m_SensorStructList[i].SensorStruct;
            i = m_SensorCount+1;
        }
    }

    return StructSensorPtr;
}

// -----------------------------------------------------
// Function Name: Sensor_GetData
// Description: Get sensor data from Sensor Structure
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. StructSensorPtr: Pointer of Sensor structure which selectd by Sensor ID
//
// Return:  Ref enum eSenRet, Sensor Functions Return Code
//
eSenRet Sensor_GetData(INT16 SensorId, SenStrcut_t *StructSensorPtr)
{
    eSenRet RetCode = eSenRetFail;

    if(SensorId>SENSOR_STRUCTURE_LIST_MAX || StructSensorPtr==NULL)
        return RetCode;

    //switch(m_SensorStructList[SensorId].SenType)
    switch(StructSensorPtr->sid)
    {
        //case eTypeTemperature:
        case 3303:  //  Temperature
        StructSensorPtr->Data.fVal = m_SensorStructList[SensorId].SensorStruct.Data.fVal;
        RetCode = eSenRetOk;
        break;
        //case eTypeMagnetic:
        case 3314: //  Magnetic
        StructSensorPtr->x = m_SensorStructList[SensorId].SensorStruct.x;
        StructSensorPtr->y = m_SensorStructList[SensorId].SensorStruct.y;
        StructSensorPtr->z = m_SensorStructList[SensorId].SensorStruct.z;
        StructSensorPtr->dir = m_SensorStructList[SensorId].SensorStruct.dir;
        RetCode = eSenRetOk;
        break;
        //case eTypeAccelerometer:
        case 3313:  //  Accelerometer
        StructSensorPtr->x = m_SensorStructList[SensorId].SensorStruct.x;
        StructSensorPtr->y = m_SensorStructList[SensorId].SensorStruct.y;
        StructSensorPtr->z = m_SensorStructList[SensorId].SensorStruct.z;
        StructSensorPtr->dir = m_SensorStructList[SensorId].SensorStruct.dir;
        RetCode = eSenRetOk;
        break;
        //case eTypeLightSensor:
        case 3301:  //  Light Sensor
        StructSensorPtr->Data.iVal = m_SensorStructList[SensorId].SensorStruct.Data.iVal;
        RetCode = eSenRetOk;
        break;
        //case eTypeDigitalOutput: // Digital Output 3201
        case 3201:  //   Digital Output
        StructSensorPtr->Data.bVal = m_SensorStructList[SensorId].SensorStruct.Data.bVal;
        RetCode = eSenRetOk;
        break;
        //case eTypeDigitalInput: // Digital Input  3200
        case 3200:  //   Digital Input
        StructSensorPtr->Data.bVal = m_SensorStructList[SensorId].SensorStruct.Data.bVal;
        RetCode = eSenRetOk;
        break;
        //case eTypeCO2:          //		CO2            9001
        case  9001: //	CO2        
        StructSensorPtr->Data.fVal = m_SensorStructList[SensorId].SensorStruct.Data.fVal;
        RetCode = eSenRetOk;
        break;
        //case eTypeCO:         //  CO             9002
        case 9002:  //  CO
        StructSensorPtr->Data.iVal = m_SensorStructList[SensorId].SensorStruct.Data.iVal;
        RetCode = eSenRetOk;
        break;
        //case eTypeUltrasonic: Ultrasonic     9200
        case 9200:  //  Ultrasonic
        StructSensorPtr->Data.iVal = m_SensorStructList[SensorId].SensorStruct.Data.iVal;
        RetCode = eSenRetOk;
        break;
        //case eTypeHumidity:   //  Humidity       3304
        case 3304:  //  Humidity
        StructSensorPtr->Data.fVal = m_SensorStructList[SensorId].SensorStruct.Data.fVal;
        RetCode = eSenRetOk;
        //case eTypeUnknown:
        default:
        break;
    }
    return RetCode;
}

// -----------------------------------------------------------
// Function Name: Sensor_SetData
// Description: Set sensor value to sensor structure
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. StructSensorPtr: Set sensor value by this structure 
//
// Return:  If successful return eSenRetOk (0), else eSenRetFail (-1)
//
eSenRet Sensor_SetData(INT16 SensorId, SenStrcut_t *StructSensorPtr)
{
    eSenRet RetCode = eSenRetFail;
    INT16 sid;

    if(SensorId>SENSOR_STRUCTURE_LIST_MAX || StructSensorPtr==NULL)
        return RetCode;
    
    sid = m_SensorStructList[SensorId].SensorStruct.sid;
    
    if(sid<=0)
        return RetCode;

    //switch((unsigned char) m_SensorStructList[SensorId].SenType)
    switch(sid)
    {
        case    3303:   //  Temperature   
        m_SensorStructList[SensorId].SensorStruct.Data.fVal = StructSensorPtr->Data.fVal;
        RetCode = eSenRetOk;
        break;
        case    3304:   //  Humidity:
        m_SensorStructList[SensorId].SensorStruct.Data.fVal = StructSensorPtr->Data.fVal;
        RetCode = eSenRetOk;
        break;
        case    3314:   //  Magnetic
        m_SensorStructList[SensorId].SensorStruct.x = StructSensorPtr->x;
        m_SensorStructList[SensorId].SensorStruct.y = StructSensorPtr->y;
        m_SensorStructList[SensorId].SensorStruct.z = StructSensorPtr->z;
        m_SensorStructList[SensorId].SensorStruct.dir = StructSensorPtr->dir;
        RetCode = eSenRetOk;
        break;
        case    3313:   // Accelerometer
        m_SensorStructList[SensorId].SensorStruct.x = StructSensorPtr->x;
        m_SensorStructList[SensorId].SensorStruct.y = StructSensorPtr->y;
        m_SensorStructList[SensorId].SensorStruct.z = StructSensorPtr->z;
        m_SensorStructList[SensorId].SensorStruct.dir = StructSensorPtr->dir;
        RetCode = eSenRetOk;
        break;
        case    3301:    // Light Sensor 
        m_SensorStructList[SensorId].SensorStruct.Data.iVal = StructSensorPtr->Data.iVal;
        RetCode = eSenRetOk;
        break;
        case    3201:    //  Digital Output
        m_SensorStructList[SensorId].SensorStruct.Data.bVal = StructSensorPtr->Data.bVal;
        RetCode = eSenRetOk;
        break;
        case    3200:     //  Digital Input
        m_SensorStructList[SensorId].SensorStruct.Data.bVal = StructSensorPtr->Data.bVal;
        RetCode = eSenRetOk;
        break;
        case    9001:     //  CO2
        m_SensorStructList[SensorId].SensorStruct.Data.fVal = StructSensorPtr->Data.fVal;
        RetCode = eSenRetOk;
        break;
        case    9002:     //  CO
        m_SensorStructList[SensorId].SensorStruct.Data.iVal = StructSensorPtr->Data.iVal;
        RetCode = eSenRetOk;
        break;
        case    9200:    //  Ultrasonic:
        m_SensorStructList[SensorId].SensorStruct.Data.iVal = StructSensorPtr->Data.iVal;
        RetCode = eSenRetOk;
        break;
        case eTypeUnknown:
        default:
        break;
    }

    return RetCode;
}

// -----------------------------------------------------------
// Function Name: Sensor_SetHostName
// Description: Set Sensor Host Name
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. HostName: Host name with ASCII string
//
// Return:  If successful return length of string, else -1
//
INT16  Sensor_SetHostName(INT16 SensorId, const INT8 *HostName)
{
    int len = -1;
    
    if(SensorId<0 && SensorId>m_SensorCount && HostName==NULL)
        return len;
    
    len = strlen((char*)HostName);    
    if(len>0 && len<SENSOR_HOSTNAME_MAX)
    {
        memset(m_SensorStructList[SensorId].SensorStruct.hostname,0 , SENSOR_HOSTNAME_MAX);
        strncpy((char*)m_SensorStructList[SensorId].SensorStruct.hostname,(char*)HostName,len);
    }

    return len;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetHostName
// Description: Get Sensor Host Name by selected Sensor ID
// Pararmeters:
//           1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Host Name with Ascii string, else NULL
//
INT8* Sensor_GetHostName(INT16 SensorId)
{
    
    if(SensorId>=0 && SensorId<=m_SensorCount)
    {
        return m_SensorStructList[SensorId].SensorStruct.hostname;
    }

    return  NULL;
}

// -----------------------------------------------------------
// Function Name: Sensor_SetModelName
// Description: Set Sensor Model Name by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. ModelName: Sensor Model Name with ASCII string
//
// Return:  If successful return length of string, else -1
//
INT16  Sensor_SetModelName(INT16 SensorId, const INT8 *ModelName)
{
    INT16 len = -1;
    
     if(SensorId<0 && SensorId>m_SensorCount && ModelName==NULL)
        return len;
    
    len = strlen((char*)ModelName);    
    if(len>0 && len<SENSOR_MODELNAME_MAX)
    {
        memset(m_SensorStructList[SensorId].SensorStruct.modelname ,0 , SENSOR_MODELNAME_MAX);
        strncpy((char*)m_SensorStructList[SensorId].SensorStruct.modelname ,(char*)ModelName, len);
    }

    return len;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetModelName
// Description: Get Sensor Model Name by selectd Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Model Name, else NULL
//
INT8* Sensor_GetModelName(INT16 SensorId)
{
    if(SensorId>=0 && SensorId<=m_SensorCount)
    {
        return m_SensorStructList[SensorId].SensorStruct.modelname;
    }

    return NULL;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetSensorName
// Description: Get Sensor Name by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Name with Ascii string, else return NULL
//
INT8* Sensor_GetSensorName(INT16 SensorId)
{
    INT16 Size = sizeof(m_SensorName)-1;
    UINT16 Sid;
    INT16 i;
        
    if(SensorId>=0 && SensorId<=m_SensorCount)
    {
        if(m_SensorStructList[SensorId].SenType>=0 && m_SensorStructList[SensorId].SenType<eTypeMax)
        {
            Sid = (UINT16) m_SensorStructList[SensorId].SenType;
        
            for(i=0;i<Size;i++)
            {
                if(m_SensorName[i].sid==Sid)
                    return (INT8 *) m_SensorName[i].sensorName;
            }    
        }
    }

    return NULL;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetSmid
// Description: Get Sensor Smart Object ID by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Smart Object Id , else -1
//
INT16 Sensor_GetSmid(INT16 SensorId)
{
    if(SensorId>=0 && SensorId<=m_SensorCount)
    {
        return m_SensorStructList[SensorId].SensorStruct.sid;
    }

    return -1;
}

// -----------------------------------------------------------
// Function Name: Sensor_SetSmid
// Description: Set Sensor Smart Object ID
// Pararmeters:
//          1. SensorId: Sensor ID
//          2. Sid: Set Smart Object ID 
//
// Return:  If successful return 0, else -1
//
INT16 Sensor_SetSmid(INT16 SensorId, INT16 Sid)
{
    if(SensorId>=0 && SensorId<=m_SensorCount && Sid>0)
    {
        m_SensorStructList[SensorId].SensorStruct.sid = Sid;
        return 0;
    }

    return -1;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetType
// Description: Get Sensor Type by selected Sensor ID
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Type, else eTypeUnknown=-1
// 
eSenType Sensor_GetType(INT16 SensorId)
{
    if(SensorId>=0 && SensorId<=m_SensorCount)
    {
        return m_SensorStructList[SensorId].SenType;
    }

    return eTypeUnknown;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetDataType
// Description: Get Sensor Data Type by selected Sensor ID 
// Pararmeters:
//          1. SensorId: Sensor ID
//
// Return:  If successful return Sensor Data Type, else eDataTypeUnKnown=0
//
eDataType Sensor_GetDataType(INT16 SensorId)
{
    if(SensorId>=0 && SensorId<=m_SensorCount)
    {
        return m_SensorStructList[SensorId].SensorStruct.DataType;
    }

    return eDataTypeUnKnown;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetCount
// Description: Get Sensor count, that have add to system
// Pararmeters: None
//
// Return:  return numbers of Sensor
//
INT16 Sensor_GetCount(void)
{
    return m_SensorCount;
}

// -----------------------------------------------------------
// Function Name: Sensor_ClearCount
// Description: Clean Sensor Count
// Pararmeters: None
//
// Return:  None
//
void Sensor_ClearCount(void)
{
    m_SensorCount = 0;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetSmidBySenType
// Description: Get Sensor Smart Object ID by Sensor Type
// Pararmeters:
//          1. eSensorType: Sensor Type
//
// Return:  If successful return IPSO Smart Object ID, else eTypeUnknown or -1
//
INT16 Sensor_GetSmidBySenType(eSenType eSensorType)
{
    INT16 retval = -1;

    switch(eSensorType)
    {
        case eTypeTemperature:
        retval = 3303;
        break;
        case eTypeHumidity:
        retval = 3304;
        break;
        case eTypeMagnetic:
        retval = 3314;
        break;
        case eTypeAccelerometer:
        retval = 3313;
        break;
        case eTypeLightSensor:
        retval = 3301;
        break;
        case eTypeDigitalOutput: // Set
        retval = 3201;
        break;
        case eTypeDigitalInput: // Get
        retval = 3200;
        break;
        case eTypeCO2:
        retval = 9001;
        break;
        case eTypeCO:
        retval = 9002;
        break;
        case eTypeUltrasonic:
        retval = 9200;
        break;
        case eTypeUnknown:
        default:
        break;
    }

    return retval;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetIdByHostname
// Description: Get Sensor ID by search HostName
// Pararmeters:
//          1. Hostname: Sensor Host Name
//
// Return:  If successful return Sensor ID, else -1
//
INT16 Sensor_GetIdByHostname(INT8 *Hostname)
{
    INT16 i;
    
    for(i=0;i<m_SensorCount;i++)
    {
        if(strcmp((char*)m_SensorStructList[i].SensorStruct.hostname, (char*)Hostname)==0)
            return i;
    }
    
    return -1;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetIdByModelname
// Description: Get Sensor Id by search Modelname
// Pararmeters:
//          1. Modelname: Sensor Model Name
//
// Return:  If successful return Sensor ID, else -1
//
INT16 Sensor_GetIdByModelname(INT8 *Modelname)
{
    INT16 i;
        
    for(i=0;i<m_SensorCount;i++)
    {
        if(strcmp((char*)m_SensorStructList[i].SensorStruct.modelname, (char*)Modelname)==0)
            return i;
    }
    
    return -1;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetUnit
// Description: Get Mote Unit string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object ID
//
// Return:  If successful return IPSO unit string, else -1
//
INT8* Sensor_GetUnit(INT16 Sid)
{
    INT16 size = sizeof(m_Standards)-1;
    INT16 i = 0;
    
    for(i = 0 ; i < size ; i++)
	{
      if(Sid == m_Standards[i].sid)
    	  return (INT8 *)m_Standards[i].unit;
    }
    
    return NULL;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetStandard
// Description: Get Mote Standard string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object Id
//
// Return:  If successful return IPSO standard string, else -1
//
INT8* Sensor_GetStandard(INT16 Sid)
{
    INT16 size = sizeof(m_Standards)-1;
    INT16 i = 0;
   
    for(i = 0 ; i < size ; i++)
	{
		if(Sid == m_Standards[i].sid)
			return (INT8 *)m_Standards[i].standard;
    }
    
    return NULL;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetResourceType
// Description: Get Mote Resource Type string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object Id
//
// Return:  If successful return IPSO resource type string, else -1
//
INT8* Sensor_GetResourceType(INT16 Sid)
{
    INT16 size = sizeof(m_Standards)-1;
    INT16 i = 0;
    
    for(i = 0 ; i < size ; i++)
	{
		if(Sid == m_Standards[i].sid)
			return (INT8 *)m_Standards[i].resourceType;
    }
    
    return NULL;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetAccessMode
// Description: Get Mote Access Mode string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object Id
//
// Return:  If successful return IPSO Access Mode string, else -1
//
INT8* Sensor_GetAccessMode(INT16 Sid)
{
    INT16 size = sizeof(m_Standards)-1;
    INT16 i = 0;

    for(i = 0 ; i < size ; i++)
	{
		if(Sid == m_Standards[i].sid)
			return (INT8 *)m_Standards[i].accessMode;
    }
    
    return NULL;
}

// -----------------------------------------------------------
// Function Name: Sensor_GetDatatype
// Description: Get Sensor Data Type string by Smart Object ID
// Pararmeters:
//          1. Sid: Smart Object Id
//
// Return:  If successful return IPSO Data Type string, else -1
//
INT8* Sensor_GetDataTypeBySid(INT16 Sid)
{
    INT16 size = sizeof(m_Standards)-1;
    INT16 i = 0;

    for(i = 0 ; i < size ; i++)
	{
		if(Sid == m_Standards[i].sid)
			return (INT8 *)m_Standards[i].dataType;
    }
    
    return NULL;
}

#if defined(TEST_EMULATOR)

#include <board/wise_1020/config.h>

#if defined  (CO2_BOARD)
void Sensor_Temperature(void)
{
    INT16 g_u8TemperatureID;
    SenStrcut_t tSensor;

    memset(&tSensor, 0, sizeof(SenStrcut_t));
    tSensor.DataType = eDataTypeFloat;
    tSensor.Data.fVal = 0;
    tSensor.max = 100;
    tSensor.min = 0;
    g_u8TemperatureID = Sensor_AddDevice(3303, &tSensor);  //   Temperature    3303
    Sensor_SetHostName(g_u8TemperatureID, (const INT8 *)TEMPERATURE_HOSTNAME);
    Sensor_SetModelName(g_u8TemperatureID, (const INT8 *)TEMPERATURE_MODELNAME);
}

void Sensor_Humidity(void)
{
    INT16 g_u8HumidityID;
    SenStrcut_t tSensor;

    memset(&tSensor, 0, sizeof(SenStrcut_t));
    tSensor.DataType = eDataTypeFloat;
    tSensor.Data.fVal = 0;
    tSensor.max = 100;
    tSensor.min = 0;
    g_u8HumidityID = Sensor_AddDevice(3304, &tSensor);     //  Humidity       3304
    Sensor_SetHostName(g_u8HumidityID, (const INT8 *)HUMIDITY_HOSTNAME);
    Sensor_SetModelName(g_u8HumidityID, (const INT8 *)HUMIDITY_MODELNAME);
}

void Sensor_CO2(void)
{
    INT16 g_u8Co2ID;
    SenStrcut_t tSensor;

    memset(&tSensor, 0, sizeof(SenStrcut_t));
    tSensor.DataType = eDataTypeFloat;
    tSensor.Data.fVal = 0;
    tSensor.max = 2000;
    tSensor.min = 0;
    g_u8Co2ID = Sensor_AddDevice(9001, &tSensor);   //  CO2            9001
    Sensor_SetHostName(g_u8Co2ID, (const INT8 *)CO2_HOSTNAME);
    Sensor_SetModelName(g_u8Co2ID, (const INT8 *)CO2_MODELNAME);
}
#elif defined(MOTION_BOARD)
void Sensor_Accel(void)
{
    INT16 g_u8AccelerID;
    SenStrcut_t tSensor;

    memset(&tSensor, 0, sizeof(SenStrcut_t));
    tSensor.DataType = eDataTypeFloat;
    tSensor.x = 0;
    tSensor.y = 0;
    tSensor.z = 0;
    g_u8AccelerID = Sensor_AddDevice(3313, &tSensor);   //  Accelerometer  3313
    Sensor_SetHostName(g_u8AccelerID, (const INT8 *) ACCEL_HOSTNAME);
    Sensor_SetModelName(g_u8AccelerID, (const INT8 *) ACCEL_MODELNAME);
}

void Sensor_eCompass(void)
{
    INT16 g_u8MagnetID;
    SenStrcut_t tSensor;

    memset(&tSensor, 0, sizeof(SenStrcut_t));
    tSensor.DataType = eDataTypeFloat;
    tSensor.x = 0;
    tSensor.y = 0;
    tSensor.z = 0;    
    g_u8MagnetID = Sensor_AddDevice(3314, &tSensor);   //  Magnetic       3314
    Sensor_SetHostName(g_u8MagnetID, (const INT8 *)ECOMPASS_HOSTNAME);
    Sensor_SetModelName(g_u8MagnetID, (const INT8 *)ECOMPASS_MODELNAME);
}
#elif defined(AGRICULTURE_BOARD)
void Sensor_BroadbandLight(void)
{
    INT16 g_uBroadbandLightID;
    SenStrcut_t tSensor;
    tSensor.DataType = eDataTypeS32;
    tSensor.Data.iVal = 0;
    tSensor.max = 2000;
    tSensor.min = 0;    
    memset(&tSensor, 0, sizeof(SenStrcut_t));
    g_uBroadbandLightID = Sensor_AddDevice(3301, &tSensor);    //  Light Sensor   3301
    Sensor_SetHostName(g_uBroadbandLightID, (const INT8 *)ECOMPASS_HOSTNAME);
    Sensor_SetModelName(g_uBroadbandLightID, (const INT8 *)ECOMPASS_MODELNAME);
}

void Sensor_InfraredLight(void)
{
    INT16 g_uInfraredLightID;
    SenStrcut_t tSensor;
    tSensor.DataType = eDataTypeS32;
    tSensor.Data.iVal = 0;
    tSensor.max = 400;
    tSensor.min = 0;    
    memset(&tSensor, 0, sizeof(SenStrcut_t));
    g_uInfraredLightID = Sensor_AddDevice(3301, &tSensor);    //  Light Sensor   3301
    Sensor_SetHostName(g_uInfraredLightID, (const INT8 *)ECOMPASS_HOSTNAME);
    Sensor_SetModelName(g_uInfraredLightID, (const INT8 *)ECOMPASS_MODELNAME);
}
#endif
#if defined(IO_BOARD)
void Sensor_DigitalOutput(void)
{
    INT16 g_uDigitalOutputID;
    SenStrcut_t SenStruct;

    memset(&SenStruct, 0, sizeof(SenStrcut_t));
    SenStruct.DataType = eDataTypeBoolean;
    g_uDigitalOutputID =  Sensor_AddDevice(3201, &SenStruct);    //  Digital Output 3201
    Sensor_SetHostName(g_uDigitalOutputID, "DigitalOutput");
    Sensor_SetModelName(g_uDigitalOutputID, "Digital Output");
}

void Sensor_DigitalInput(void)
{
    INT16 g_uDigitalInputID;
    SenStrcut_t SenStruct;

    memset(&SenStruct, 0, sizeof(SenStrcut_t));
    SenStruct.DataType = eDataTypeBoolean;
    g_uDigitalInputID =  Sensor_AddDevice(3200, &SenStruct);    //  Digital Input  3200
    Sensor_SetHostName(g_uDigitalInputID, "DigitalInput");
    Sensor_SetModelName(g_uDigitalInputID, "Digital Input");
}
#endif
void Sensor_Test(void)
{
    // Add Sensor
#if defined  (CO2_BOARD)
    Sensor_Temperature();
    Sensor_Humidity();
    Sensor_CO2();
#elif defined(MOTION_BOARD)
    Sensor_Accel();
    Sensor_eCompass();
#elif defined(AGRICULTURE_BOARD)
    Sensor_BroadbandLight();
    Sensor_InfraredLight();
#endif
#if defined(IO_BOARD)
    Sensor_DigitalOutput();
    Sensor_DigitalInput();
#endif

}
#endif

