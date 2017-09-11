
//*****************************************************************************
//                        ------ Main Function ------
//
// Description  : 1. Main flowchart of WISE-1540
//                2. Communication with WISE-PaaS
//				  3. CLI for API interface
//                4. Back-up to default setting
//
// Company      : Advantech Co., Ltd.
// Department   : WISE-IoT
// Author       : Will Chen
// Create Date  : 2017/07/03
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (C) 2017 Advantech Co., Ltd. All Rights Reserved
//
//*****************************************************************************

//*****************************************************************************
//                 Included files
//*****************************************************************************
#include "mbed.h"
#include "common/common.h"
#include "wsn/sm_ipmt/mote_api.h"
#include "wsn/sm_ipmt/mote_command_set.h"
#include "wsn/sm_ipmt/board.h"
#include "board/wise_1540/config.h"
#include "datafmt/ipso/Parser.h"
#include "datafmt/ipso/SensorFormat.h"
#include "common/console.h"
#include "sensor/ti/hdc1050.h"



//*****************************************************************************
//                 MACROs and needed structures
//*****************************************************************************
//
// About flowchart of state machine
//
#define SCHEDULE_TIME   		1000 //ms
#define RESEND_MAX_NUM          2
#define AUTOCMD_SCHEDULE_TIME   2

//
// Setting about reset to default
//
#define SW_VERSION_MAJOR		0
#define SW_VERSION_MINOR		1
#define SW_VERSION_PATCH		0
#define MODEL_NAME   	   		"WISE-1540"
#define DEF_JOINKEY   	    	"JOINADVANTECHIOT"
#define DEF_NETID    	    	2001U
#define DEF_AUTOJOIN			1
#define DEF_ROUTING_MODE		0
#define DEF_TX_POWER			8
#define DEF_ANTGAIN				2
#define DEF_JOIN_DUTY_CYCLE		64U
#define DEF_EVENT_MASK			0xFFFFFFFF
#define DEF_EUCOMPLIANT_MODE 	0U

//
// Definition about sensor
//
#define USE_HDC1050

//
// About connectivity
// Payload Size Limits with Linear dust smartmesh IP mote connectivity
//
//+---------------------------+------------+------------------+
//| Src, Dst Ports            | To Manager | To Other Address |
//+---------------------------+------------+------------------+
//| Both are F0Bx             | 90 bytes   | 74 bytes         |
//| Either (or both) are F0xx | 88 bytes   | 72 bytes         |
//| Any other                 | 87 bytes   | 71 bytes         |
//+---------------------------+------------+------------------+
//
#define DUST_PAYLOAD_SIZE	87



//*****************************************************************************
//                 Global variables
//*****************************************************************************
static UINT8 g_au8Payload[DUST_PAYLOAD_SIZE]  __attribute__ ((aligned (4)));;
static UINT8 g_u8SocketID = 0;
static INT16 g_Reset = 0;
static INT16 g_TemperatureID = 0;
static float g_fTemperature = 0;
#if defined(USE_HDC1050)
static INT16 g_HumidityID = 0;
static UINT16 g_u16Humidity;
#endif

//
// About I/O
//
static DigitalIn g_tBackUp(BACKUP);
static DigitalOut g_tPower(CB_PWR_ON);
static DigitalInOut g_tGPIO0(GPIO0);



//*****************************************************************************
//
//! \brief Create connection to manager
//!
//! \param   none
//!
//! \return  On success, zero is returned. 
//!          On error, negative is returned.
//!
//*****************************************************************************
static INT8 Main_i8CreateConnection(void)
{
    UINT8 ui8Status;
    UINT32 ui32Event;
    
    //
	// Connect to smart mesh IP manager when smart mesh IP mote get ready
	//
	mote_api_get_state(&ui8Status);
    mote_api_get_event(&ui32Event);

	if(ui8Status == MOTE_API_ST_OPERATIONAL) {
        mote_api_autojoin();
        mote_api_get_socketid((INT8*)&g_u8SocketID);
        return 0;
    }

    return -1;
}



//*****************************************************************************
//
//! \brief Check whether connection is created
//!
//! \param   none
//!
//! \return  The connection is created, TRUE is returned. 
//!          No connection, FALSE is returned.
//!
//*****************************************************************************
static BOOL Main_bChkConnection(void)
{
    UINT8 ui8State;

	mote_api_get_state(&ui8State);
    if(ui8State != MOTE_API_ST_OPERATIONAL) {
        return FALSE; 
    }

    return TRUE;
}



//****************************************************************************
//
//! \brief Send data to manager
//!
//! \param _pu8Src pointer to data source
//! \param _u8Size the size of data source
//!
//!
//! \return  Please refer to 'ErrorSMIPMote' in the mote_api.h for response
//!			 code.
//
//****************************************************************************
static INT8 Main_i8SendData(UINT8 *_pu8Src, UINT8 _u8Size)
{
    static UINT16 u16PacketID = 0;

    if(u16PacketID >= 1000)
    	u16PacketID = 0;

    return mote_api_send_to(g_u8SocketID, 
              					MANAGER_DEST_IP_TOP, 
              					MANAGER_DEST_IP_BOT,
              					PORT_NUM,
              					MOTE_API_SERVICE_TYPE_BW, MOTE_API_PRIORITY_MED, 
              					u16PacketID++,
              					_pu8Src, 
                                (_u8Size > DUST_PAYLOAD_SIZE)? DUST_PAYLOAD_SIZE: _u8Size);
}



//*****************************************************************************
//
//! \brief Update sensor data
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void Main_i8UpdateSensorData(void)
{
#if defined(USE_HDC1050)
    HDC1050_GetSensorData(&g_fTemperature, &g_u16Humidity);
#else
    static float fFakeTemp = 0.1;
    float fTemperature;

    //
    // Update data for fake temperature senosr
    //
    if(fFakeTemp >= 5) 
        fFakeTemp = 0;

    fTemperature = 30 + (fFakeTemp += 0.1);
    g_fTemperature = fTemperature;
#endif // endif USE_HDC1050
}



//*****************************************************************************
//
//! \brief Reset MCU
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void Main_CmdReset(void)
{    
	//	
	// PUT /dev/info?reset=1
	//
	mote_api_cmd_reset();
	wait_ms(2000);

	//
	// Reset MCU
	//
	HAL_NVIC_SystemReset();
}



//*****************************************************************************
//
//! \brief Update information by specified command
//!
//! \param  _i8Cmd the command with related information
//!
//! \return  On success, zero is returned. 
//!
//*****************************************************************************
static INT8 Main_i8UpdateInfoForCmd(INT8 _i8Cmd)
{
    SenStrcut_t tSensor;
	BoardConfig_tp ptBoardCfg;
	CoapAction_tp  ptCoapAction;
	INT16 len;
	
	ptCoapAction = Paser_GetCoapActionPtr();
	ptBoardCfg = Board_GetCfgPtr();
	len = 0;
	
    switch(_i8Cmd)
    {
	    case eURICmdSenInfoListAll:
            break;              

        case eURICmdSenInfoListFieldId:
		    break;  

        case eURICmdSenInfoList:
		    break;

        case eURICmdQuerySenBySid:
		    break;

        case eURICmdQuerySenById:
		    break;

        case eURICmdObserve:	        // Observe (MCU:Update all)
            memset(&tSensor, 0, sizeof(SenStrcut_t));
            tSensor.Data.fVal = g_fTemperature;
            Sensor_SetData(g_TemperatureID, &tSensor);
#if defined(USE_HDC1050)
            tSensor.Data.fVal = (float)g_u16Humidity;
            Sensor_SetData(g_HumidityID, &tSensor);
#endif // endif USE_HDC1050
            break;		
        case eURICmdCancel:    	        // Observe (MCU:Don't update)
	        break;

        case eURICmdDevInfoList:		// GET /dev/info?action=list
            break;

        case eURICmdDevStatus:
            break;

        case eURICmdSetDevName:		    // PUT /dev/info?n="Demo1"
		    //	
		    // Use Tasklet or post to do burn flash
		    //
		    if(ptCoapAction!=NULL) {
			    if(ptCoapAction->reg.dev==1 && ptCoapAction->reg.name==1) {
				    len = strlen((char*)ptCoapAction->name);
				    if(len>0 && len<BOARD_NAME_MAX) {
	                    memset(ptBoardCfg->hostname, 0,BOARD_NAME_MAX);
					    strncpy((char*)ptBoardCfg->hostname, (const char*)ptCoapAction->name, len);					
					    Board_SaveConfig();
				    }
			    }
		    }
		    break;  

        case eURICmdSetSenValueById:    // PUT /sen/status?id=5&bv=1
		    break;

        case eURICmdSetSenValueByName:  // PUT	/sen/status?n=dout&bv=1
		    break;  

        case eURICmdNetInfoList:        // GET /net/info?action=list
            break;

        case eURICmdReset:			    // PUT /dev/info?reset=1
		    if(ptBoardCfg!=NULL && ptCoapAction!=NULL) {
			    if(ptCoapAction->Data.bVal)
				    g_Reset = 1;	
		  	}
		    break;

        default:
		    break;		  
    }    

    return 0;
}



//*****************************************************************************
//
//! \brief Initiation for sensors
//!
//! \param  none
//!
//! \return  On success, zero is returned. 
//!
//*****************************************************************************
static INT8 Main_i8SensorInit(void)
{
	SenStrcut_t tSensor;
	
#if defined(USE_HDC1050)
    HDC1050_Init();
#endif // endif USE_HDC1050

	Parser_Init();
	Sensor_DeviceInit();
	Board_Init();
	
	//
	// Call Sensor_AddDevice to add Sensor Device to list, 
	// Sensor max count is 16
	//
    //
    // Register for tmperature sensor
    //
   	memset(&tSensor, 0, sizeof(SenStrcut_t));
    tSensor.DataType = eDataTypeFloat;
    tSensor.max = 100;
	tSensor.min = -10;
    g_TemperatureID = Sensor_AddDevice(3303, &tSensor);
    Sensor_SetHostName(g_TemperatureID, (const INT8 *)TEMPERATURE_HOSTNAME);
    Sensor_SetModelName(g_TemperatureID, (const INT8 *)TEMPERATURE_MODELNAME);

#if defined(USE_HDC1050)
    //
    //  Register for Humidity
    //
    memset(&tSensor, 0, sizeof(SenStrcut_t));
    tSensor.DataType = eDataTypeFloat; 
    tSensor.max = 100; 
	tSensor.min = 0; 
    g_HumidityID = Sensor_AddDevice(3304, &tSensor);
    Sensor_SetHostName(g_HumidityID, (const INT8 *)HUMIDITY_HOSTNAME);
    Sensor_SetModelName(g_HumidityID, (const INT8 *)HUMIDITY_MODELNAME);  
#endif // endif USE_HDC1050

    return 0;
}



//****************************************************************************
//
//! The thread to show console for setting about connectivity
//!
//! \param none
//!
//! \return None.
//
//****************************************************************************
static void vConsoleThread(void)
{
	char aBuf[64];

	while(1) 
	{      
	    Thread::wait(100);
    	MSG_print("> ");
	    Console_iGetString(aBuf, 64);
		Console_iRunCmd(aBuf);
	}
	
}



//****************************************************************************
//
//! The thread to communication with WISE-PaaS
//!
//! \param none
//!
//! \return None.
//
//****************************************************************************
static void vCommunicationThread(void)
{
	int k;
	unsigned int j;
	BOOL bMoteInit = FALSE;
	BOOL bRecvAutoCmd;
    UINT8 u8Size = 0;
    UINT8 u8SendCnt = 0;
    UINT8 u8AutoCmdCnt = 0;
    INT8 i8RunCmd, i8RunAutoCmd;
    INT16 i16Ret;
    eURICmd eURICmdCode;
	
	//
    // Forever loop
    //
    i8RunCmd = eURICmdUnKnown;
    i8RunAutoCmd = eURICmdUnKnown;
	j=0;
    while(1)
    {
		DBG_print("Loop......[%d]\r\n", j++);
		wait_ms(SCHEDULE_TIME);
		Main_i8UpdateSensorData();
	
        if(bMoteInit == FALSE) {
            if(Main_i8CreateConnection() != 0) {
				DBG_print("Create connection.......\r\n");
                continue;
            }
            else {
                bMoteInit = TRUE;
                DBG_print("Connection to manager successfully.\r\n");
            }       
        }

        if(Main_bChkConnection() == FALSE) {
        	bMoteInit = FALSE;
            DBG_print("Disconnect.\r\n");
            continue;
        }

        //
        // Update data for h/w sensor      
        //
        Main_i8UpdateSensorData();

        //
        // Receive command from smartmesh IP manager
        //
        eURICmdCode = eURICmdUnKnown;
        if(mote_api_receive_trigger()==TRUE && g_Reset==0) {
			DBG_print("Receive data from manager and it's ready to decode.\r\n");
			for(k=8; k < (mote_api_get_payload_size() - 8); k++) {
				if((k+1) % 8 == 0)
					DBG_print("%c \r\n", g_au8Payload[k]);
				else
					DBG_print("%c ", g_au8Payload[k]);
			}
			DBG_print("\r\n");
		    eURICmdCode = Parser_Decode((INT8*)g_au8Payload, mote_api_get_payload_size());
        }

        //
        // Check command type whether normal or auto command and set related 
        // variables for controlled flowchart
        //
        // Command types brief as below:
        // Normal command: all commands are normal command except eURICmdObserve
        //                 and eURICmdCancel
        // Auto command  : eURICmdObserve and eURICmdCancel
        //
        bRecvAutoCmd = FALSE;
        if(eURICmdCode != eURICmdUnKnown) {
            if(eURICmdCode != eURICmdObserve && eURICmdCode != eURICmdCancel) {
                i8RunCmd = eURICmdCode;
            }
            else {
                i8RunAutoCmd = eURICmdCode;
                bRecvAutoCmd = TRUE;
            }
        }

        //
        // Response to smartmesh IP manager after received valid "normal" command
        //
        if(i8RunCmd != eURICmdUnKnown) {  
		    Main_i8UpdateInfoForCmd(i8RunCmd);
            u8Size = (UINT8)Parser_Encode((eURICmd)i8RunCmd, (INT8*)g_au8Payload, sizeof(g_au8Payload));
            i8RunCmd = eURICmdUnKnown;
            u8SendCnt = 0;
            
            while(1)
            {
            	DBG_print("Send data to manager.\r\n");
				i16Ret = (INT16)Main_i8SendData((UINT8*)g_au8Payload, u8Size);
                
                if(g_Reset==1)
               		Main_CmdReset();
               	
                if(i16Ret == eMt_OK) 
                	break;
                	
                if(u8SendCnt >= RESEND_MAX_NUM) {
                    u8SendCnt = 100; 
                    break;
                }
                else {
                    u8SendCnt++;
                }
                
                if(Main_bChkConnection() == FALSE) {
                	bMoteInit = FALSE;
                	DBG_print("Disconnect.\r\n");
                    break;
                }
            }
        }          

        //
        // Response to smartmesh IP manager after received valid "auto" command
        //
        if(i8RunAutoCmd != eURICmdUnKnown && bMoteInit == TRUE) {
            if(u8AutoCmdCnt < AUTOCMD_SCHEDULE_TIME && bRecvAutoCmd != TRUE ) {
                u8AutoCmdCnt++;
                continue;
            }
            else {
                u8AutoCmdCnt = 0;
            }
            
		    Main_i8UpdateInfoForCmd(i8RunAutoCmd);
            u8Size = (UINT8)Parser_Encode((eURICmd)i8RunAutoCmd, (INT8*)g_au8Payload, sizeof(g_au8Payload));
            if(i8RunAutoCmd == eURICmdCancel)
                i8RunAutoCmd = eURICmdUnKnown;
            u8SendCnt = 0;
            
            while(1)
            {
            	DBG_print("Send data to manager in auto mode.\r\n");
				i16Ret = Main_i8SendData((UINT8*)g_au8Payload, u8Size);
                
                if(g_Reset==1)
               		Main_CmdReset();
                               
                if(i16Ret == eMt_OK) 
                	break;
                	
                if(u8SendCnt >= RESEND_MAX_NUM) {
                    u8SendCnt = 100; 
                    break;
                }
                else {
                    u8SendCnt++;
                }
                
                if(Main_bChkConnection() == FALSE) {
                	bMoteInit = FALSE;
                	DBG_print("Disconnect.\r\n");
                    break;
                }
            }
        }
    } // End forever loop while(1)
    
}



//****************************************************************************
//
//! Reset to default setting
//!
//! \param none
//!
//! \return  On success, zero is returned.
//!          On error, negative is returned.
//
//****************************************************************************
static int Main_iResetToDefault(void)
{
	int iRet;

	iRet = mote_api_setp_joindutycycle(DEF_JOIN_DUTY_CYCLE);
	if(iRet != eMt_OK) return -1;

	iRet = mote_api_setp_networkid(DEF_NETID);
	if(iRet != eMt_OK) return -2;

	iRet = mote_api_setp_autojoin((BOOL)DEF_AUTOJOIN);
	if(iRet != eMt_OK) return -3;

	iRet = mote_api_setp_txpower(DEF_TX_POWER);
	if(iRet != eMt_OK) return -4;	

	iRet = mote_api_setp_antgain(DEF_ANTGAIN);
	if(iRet != eMt_OK) return -5;

	iRet = mote_api_setp_eucompliantmode(DEF_EUCOMPLIANT_MODE);
	if(iRet != eMt_OK) return -6;

	iRet = mote_api_setp_eventmask(DEF_EVENT_MASK);
	if(iRet != eMt_OK) return -7;

	iRet = mote_api_setp_routingmode((BOOL)DEF_ROUTING_MODE);
	if(iRet != eMt_OK) return -8;
	
	iRet = mote_api_setp_joinkey((INT8*)DEF_JOINKEY);
	if(iRet != eMt_OK) return -9;

	mote_api_hw_reset();

	return eMt_OK;
}



//****************************************************************************
//
//! Main function
//!
//! \param none
//! 
//! This function  
//!    1. Invokes the communication task
//!    2. Invokes the console Task
//!
//! \return None.
//
//****************************************************************************
int main(void) 
{
	int iRet;
	Thread tCliThread, tCommThread;

	//
	// Initiation for board
	//
	g_tPower = 1;

	//
	// Initiation for console
	//
	Console_iInit();

	//
    // Initiation for smartmesh IP mote connectivity
    //
    mote_api_cmd_init();
	mote_api_set_payload_ptr((UINT8*)g_au8Payload);
	mote_api_hw_reset();

	// Display version information
    MSG_print("\f");
    MSG_print("\t\t *************************************************\n\r");
    MSG_print("\t\t\t\t");
    MSG_print("%s v%d.%d.%d", \
				MODEL_NAME, \
				SW_VERSION_MAJOR, \
				SW_VERSION_MINOR, \
				SW_VERSION_PATCH);
    MSG_print("\n\r");
    MSG_print("\t\t *************************************************\n\r");

	//
	// Running reset to default by checking 2-3 pin of SW3 on WISE-DB-1500
	//
	if(g_tBackUp.read() == 0) {
		MSG_print("Reset to default in progress... \n\r");
		wait_ms(3000);
		if((iRet = Main_iResetToDefault()))
			MSG_print("Failed, ret: %d \n\r", iRet);
		else
			MSG_print("Done. \n\r");
	}

	//
    // Initiation for sensor
	//
    Main_i8SensorInit();

	//
	// Running main thread by GPIO0
	//
	g_tGPIO0.output();
	g_tGPIO0 = 0;
	if(g_tGPIO0.read() == 0) {
		tCommThread.start(vCommunicationThread);
	}
	else {
		mote_command_set_init();
		tCliThread.start(vConsoleThread);
	}

	//
    // Forever loop
    //
	while(1) {
		wait(1);
	}

	return 0;
}

