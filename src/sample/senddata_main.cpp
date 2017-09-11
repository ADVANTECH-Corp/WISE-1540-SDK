
//*****************************************************************************
//                 Included files
//*****************************************************************************
#include "mbed.h"
#include "common/common.h"
#include "wsn/sm_ipmt/mote_api.h"


//*****************************************************************************
//                 MACROs and needed structures
//*****************************************************************************
#define SCHEDULE_TIME   1000 //ms

//
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
static UINT8 g_au8Payload[DUST_PAYLOAD_SIZE];
static UINT8 g_u8SocketID = 0;

DigitalOut power(CB_PWR_ON);
Serial pc(SERIAL_TX, SERIAL_RX);

//*****************************************************************************
//                 Local functions
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



static BOOL Main_bChkConnection(void)
{
    UINT8 ui8State;

	mote_api_get_state(&ui8State);
    if(ui8State != MOTE_API_ST_OPERATIONAL) {
        return FALSE; 
    }

    return TRUE;
}



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



int main() 
{
	int i, j;
	BOOL bMoteInit = FALSE;
    power = 1;

	pc.baud(115200);
    pc.format();

	//
    // Initiation for smartmesh IP mote connectivity
    //
    mote_api_set_payload_ptr((UINT8*)g_au8Payload);
    mote_api_cmd_init();   
	mote_api_hw_reset();

	//
    // Forever loop
    //
    j=0;
    while(1)
    {
    	pc.printf("Loop......[%d]\r\n", j++);
		wait_ms(SCHEDULE_TIME);
	
        if(bMoteInit == FALSE) {
            if(Main_i8CreateConnection() != 0) {
				pc.printf("Create connection.......\r\n");
                continue;
            }
            else {
                bMoteInit = TRUE;
                pc.printf("Connection to manager successfully.\r\n");
            }       
        }

        if(Main_bChkConnection() == FALSE) {
        	bMoteInit = FALSE;
            pc.printf("Disconnect.\r\n");
            continue;
        }

		//
        // Please fill all datas you want in g_au8Payload and send it to gateway
        //
        for(i = 0; i < DUST_PAYLOAD_SIZE; i++)
        	g_au8Payload[i] = i;
        Main_i8SendData((UINT8*)g_au8Payload, DUST_PAYLOAD_SIZE);
		pc.printf("Send data to manager.\r\n");
    }
}

