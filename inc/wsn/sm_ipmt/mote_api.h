#ifndef MOTE_API_H
#define MOTE_API_H

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

/*********************************************************/
/* Mote API commands id                                  */
/*********************************************************/
#define MOTE_API_CMD_SETPARAM                    0x01
#define MOTE_API_CMD_GETPARAM                    0x02
#define MOTE_API_CMD_JOIN                        0x06
#define MOTE_API_CMD_DISCONNECT                  0x07
#define MOTE_API_CMD_RESET                       0x08
#define MOTE_API_CMD_LOWPWRSLEEP                 0x09
#define MOTE_API_CMD_TESTRADIORX                 0x0C
#define MOTE_API_CMD_CLEARNV                     0x10
#define MOTE_API_CMD_SERVICE_REQUEST             0x11
#define MOTE_API_CMD_GET_SVC_INFO                0x12
#define MOTE_API_CMD_OPEN_SOCKET                 0x15
#define MOTE_API_CMD_CLOSE_SOCKET                0x16
#define MOTE_API_CMD_BIND_SOCKET                 0x17
#define MOTE_API_CMD_SENDTO                      0x18
#define MOTE_API_CMD_SEARCH                      0x24
#define MOTE_API_CMD_TESTRADIOTX                 0x28

/*********************************************************/
/* Mote API notifications id                             */
/*********************************************************/
#define MOTE_API_NOTIF_TIME                      0x0D
#define MOTE_API_NOTIF_EVENTS                    0x0F
#define MOTE_API_NOTIF_RECEIVED                  0x19
#define MOTE_API_NOTIF_TXDONE                    0x25
#define MOTE_API_NOTIF_ADVRX                     0x26
#define MOTE_API_NOTIF_MACRX                     0x27

/*********************************************************/
/* set/get parameters                                    */
/*********************************************************/           
#define MOTE_API_PARAM_MACADDR                       0x01
#define MOTE_API_PARAM_JOINKEY                       0x02
#define MOTE_API_PARAM_NETID                         0x03
#define MOTE_API_PARAM_TXPOWER                       0x04
#define MOTE_API_PARAM_RSVD1                         0x05
#define MOTE_API_PARAM_JOINDUTYCYCLE                 0x06
#define MOTE_API_PARAM_RSVD2                         0x07
#define MOTE_API_PARAM_RSVD3                         0x08
#define MOTE_API_PARAM_RSVD4                         0x09
#define MOTE_API_PARAM_RSVD5                         0x0A
#define MOTE_API_PARAM_EVENTMASK                     0x0B
#define MOTE_API_PARAM_MOTEINFO                      0x0C
#define MOTE_API_PARAM_NETINFO                       0x0D
#define MOTE_API_PARAM_MOTESTATUS                    0x0E
#define MOTE_API_PARAM_TIME                          0x0F
#define MOTE_API_PARAM_CHARGE                        0x10
#define MOTE_API_PARAM_TESTRADIORXSTATS              0x11
#define MOTE_API_PARAM_RSVD6                         0x12
#define MOTE_API_PARAM_RSVD7                         0x13
#define MOTE_API_PARAM_RSVD8                         0x14
#define MOTE_API_PARAM_OTAP_LOCKOUT                  0x15
#define MOTE_API_PARAM_MACMICKEY                     0x16
#define MOTE_API_PARAM_SHORTADDR                     0x17
#define MOTE_API_PARAM_IP6ADDR                       0x18
#define MOTE_API_PARAM_CCAMODE                       0x19
#define MOTE_API_PARAM_CHANNELS                      0x1A
#define MOTE_API_PARAM_ADVGRAPH                      0x1B
#define MOTE_API_PARAM_HRTMR                         0x1C
#define MOTE_API_PARAM_ROUTINGMODE                   0x1D
#define MOTE_API_PARAM_APPINFO                       0x1E
#define MOTE_API_PARAM_PWRSRCINFO                    0x1F
#define MOTE_API_PARAM_PWRCOSTINFO                   0x20
#define MOTE_API_PARAM_MOBILITY                      0x21
#define MOTE_API_PARAM_ADVKEY                        0x22
#define MOTE_API_PARAM_SIZEINFO                      0x23
#define MOTE_API_PARAM_AUTOJOIN                      0x24
#define MOTE_API_PARAM_PATHALARMGEN                  0x25
#define MOTE_API_PARAM_ANTGAIN                       0x29
#define MOTE_API_PARAM_EUCOMPLIANTMODE               0x2A

// values for protocol in MOTE_API_LOC_CMD_OPEN_SOCKET
#define MOTE_API_PROTO_UDP                           0      

// values for MOTE_API_PARAM_ROUTINGMODE
#define MOTE_API_ROUTING_ENABLED                     0
#define MOTE_API_ROUTING_DISABLED                    1

// values for MOTE_API_PARAM_MOBILITY  
#define MOTE_API_LOCATION_UNUSED                     0
#define MOTE_API_LOCATION_KNOWN                      1
#define MOTE_API_LOCATION_UNKNOWN                    2
#define MOTE_API_LOCATION_MOBILE                     3

// service type
#define MOTE_API_SERVICE_TYPE_BW                     0
#define MOTE_API_SERVICE_TYPE_LATENCY                1

// service state
#define MOTE_API_SERVICE_STATE_REQ_COMPLETED         0
#define MOTE_API_SERVICE_STATE_REQ_PENDING           1

// Packet priority in MOTE_API_LOC_CMD_SENDTO 
#define MOTE_API_PRIORITY_LOW                        0
#define MOTE_API_PRIORITY_MED                        1
#define MOTE_API_PRIORITY_HIGH                       2
#define MOTE_API_PRIORITY_CTRL                       3

// mote state in MOTE_API_PARAM_MOTESTATUS 
#define MOTE_API_ST_INIT                             0        // Initializing
#define MOTE_API_ST_IDLE                             1        // Idle, ready to be configured or join
#define MOTE_API_ST_SEARCHING                        2        // Received join command, searching for network
#define MOTE_API_ST_NEGO                             3        // Sent join request
#define MOTE_API_ST_CONNECTED                        4        // Received at least one packet from the controller
#define MOTE_API_ST_OPERATIONAL                      5        // Configured by controller and ready to send data
#define MOTE_API_ST_DISCONNECTED                     6        // Disconnected from the network
#define MOTE_API_ST_RADIOTEST                        7        // Radio test
#define MOTE_API_ST_PROMISC_LISTEN                   8        // Promisc listen

// mote event
#define MOTE_API_EVENT_BOOT                         0x0001  // The mote booted up
#define MOTE_API_EVENT_ALARM_CHANGE                 0x0002  // Alarm(s) were opened or closed
#define MOTE_API_EVENT_TIME_CHANGE                  0x0004  // UTC time-mapping on the mote changed
#define MOTE_API_EVENT_JOIN_FAILED                  0x0008  // Join operation failed
#define MOTE_API_EVENT_DISCONNECTED                 0x0010  // The mote disconnected from the network
#define MOTE_API_EVENT_OPERATIONAL                  0x0020  // Mote has connection to the network and is ready to send data
#define MOTE_API_EVENT_SERVICE_CHANGE               0x0080  // Service allocation has changed
#define MOTE_API_EVENT_JOIN_STARTED                 0x0100  // Mote started joining the network


// power limit table in MOTE_API_PARAM_PWRSRCINFO
#define MOTE_API_PWR_LIMIT_CNT                       3        // # of limits in pwrsrc
#define MOTE_API_SETNV_MAGIC_NUMBER                  0

// power cost defs for MOTE_API_PARAM_PWRCOSTINFO
#define MOTE_API_PWRCOSTINFO_MAX_UNLIM              0xFFFF
#define MOTE_API_PWRCOSTINFO_MIN_UNLIM              0

// values for MOTE_API_PARAM_CCAMODE
#define MOTE_API_CCA_MODE_OFF                        0
#define MOTE_API_CCA_MODE_ON                         1

// values for MOTE_API_PARAM_TXPOWER
#define MOTE_API_TXPOWER_PA_OFF                      0
#define MOTE_API_TXPOWER_PA_ON                       8

// mac join DC
#define MOTE_API_MAX_JOINDC                          0xFF

// values for status in MOTE_API_LOC_NOTIF_TXDONE 
#define MOTE_API_TXSTATUS_OK                         0
#define MOTE_API_TXSTATUS_FAIL                       1

// values for response code
#define MOTE_API_RESPONSE_OK                         0

// for command "send to"
#define MAX_RC_NUM          4
#define MAX_RC_TIMEOUT_MS   5000

// for command "receive"
#define MAX_RX_BUF_SZ       128
#define IPV6_ADDR_LEN       16

// infomation about connected to manager
#define MANAGER_DEST_ADDR               0xFFFE
#define MANAGER_DEST_IP_TOP             0xff02000000000000
#define MANAGER_DEST_IP_BOT             0x0000000000000002
#define SMARTMESH_PKT_TYPE_GPIO_CTRL    0x01
#define SMARTMESH_PKT_TYPE_SENSOR_DATA  0x02
#define SMARTMESH_BUFFER_LENGTH         128 //50 //40
#define PORT_NUM    60002

// for set/get parameter
#define MAC_LEN                     8
#define HW_RESET_PULL_HIGH          100 // ms
#define HW_RESET_PULL_LOW           100 // ms



//*****************************************************************************
//                 Needed structures
//*****************************************************************************
typedef struct {
    INT16 i16PacketID;
    INT8  i8TranStatus;
}TxDone_t;

typedef struct {
    UINT8     u8Cnt;
    TxDone_t  a_TxRc[MAX_RC_NUM];
}Notify_t;

typedef struct {
    INT8 i8RC;
    UINT8* pu8Data;
    UINT8 u8DataSz;
}RC_t;

typedef struct {
    UINT8* pu8Payload;
    UINT8 u8PayloadSz;
    BOOL bTrigger;
}NotifyReceive_t;

typedef struct {
    UINT8 au8Mac[MAC_LEN];
}Mac_t;

typedef struct {
    Mac_t tMac; 
    UINT16 u16MoteId;
    UINT16 u16NetworkId;
    UINT16 u16SlotSize;
}NetInfo_t;

typedef struct {
    UINT16 u16VendorId;
    UINT8 u8AppId;
    UINT8 u8Major;
    UINT8 u8Minor;
    UINT8 u8Patch;
    UINT16 u16Build;
}AppInfo_t;

typedef struct {
    UINT8 u8ApiVersion;
    UINT8 au8SerialNum[8];
    UINT8 u8HwModel;
    UINT8 u8HwRev;
    UINT8 u8SwVerMajor;
    UINT8 u8SwVerMinor;
    UINT8 u8SwVerPatch;
    UINT16 u16SwVerBuild;
    UINT8 u8BootSwVer;
}MoteInfo_t;

typedef struct {
    UINT8 u8State;
    UINT8 au8Reserved1[3];
    UINT8 u8NumParents;
    UINT32 u32Alarms;
    UINT8 u8Reserved2;
}MoteStatus_t;

typedef struct {
    UINT32 u32QTotal;
    UINT32 u32UpTime;
    INT8 i8TempInt;
    UINT8 u8TempFrac;
}Charge_t;

typedef struct {
    INT64 i64Sec;
    INT32 i32USec;
}UTC_Time_t;

typedef struct {
    UINT32 u32UpTime;
    UTC_Time_t tUtcTime;
    UINT8 au8Asn[5];
    INT16 i16AsnOffset;
}Time_t;

typedef struct {
    UINT8 u8State;
    UINT32 u32Value;
}ServiceInfo_t;

// Error code
enum ErrorSMIPMote {
    eMt_OK                  = 0,
    eMt_Err_EncPkt          = 0x1000,
    eMt_Err_RcTO            = 0x1001,            // Response timeout
    eMt_Err_TxDoneTO        = 0x1002,            // TxDone timeout
    eMt_Err_TxFail          = 0x1003,
    eMt_Err_End             = 0x1004
};

enum Flag {
    eF_Enable       = 0,
    eF_Disable,
};

enum Active {
    eAct_SetPara       = 1,
    eAct_GetPara,
};


//*****************************************************************************
//            Functions prototypes
//*****************************************************************************

//*****************************************************************************
//
//! \brief Initiation for mote
//!
//! \param  none
//!
//! \return  On success, zero is returned.
//!          On error, negative is returned.
//!
//*****************************************************************************
INT8 mote_api_cmd_init(void);

//*****************************************************************************
//
//! \brief Reset smartmesh IP mote
//!
//! \param  none
//!
//! \return  On success, zero is returned.
//!          On error, negative is returned.
//!
//*****************************************************************************
INT16 mote_api_cmd_reset(void);

//*****************************************************************************
//
//! \brief Send a packet into the network
//!
//! \param  socketId: Socket ID
//!         destIpTop: Top of destination IPV6 address
//!         destIpBot: Bottom of destination IPV6 address
//!         port: Destination port
//!         serviceType: Service type
//!         priority: Priority of the packet
//!         pktId: User-defined packet ID
//!         payload: Payload of the packet
//!         length: Length of payload
//!
//! \return  On success, zero is returned.
//!          On error, negative is returned.
//!
//*****************************************************************************
INT16 mote_api_send_to(UINT8 socketId, UINT64 destIpTop, UINT64 destIpBot,
                    UINT16 port, UINT8 serviceType, UINT8 priority, UINT16 pktId, 
                    UINT8* payload, UINT8 length);

//*****************************************************************************
//
//! \brief Set pointer to the payload we want to send to network
//!
//! \param  _pui8Payload: pointer to the payload
//!
//! \return  None
//!
//*****************************************************************************
void mote_api_set_payload_ptr(UINT8 *_pui8Payload);

//*****************************************************************************
//
//! \brief obtain size of received data
//!
//! \param  None
//!
//! \return  On success, size of received data is returned.
//!          On error, negative is returned.
//!
//*****************************************************************************
UINT8 mote_api_get_payload_size(void);

//*****************************************************************************
//
//! \brief This function is used to check whether packet is received from network
//!
//! \param  None
//!
//! \return  TRUE, the packet is received completely.
//!          FALSE, others.
//!
//*****************************************************************************
BOOL mote_api_receive_trigger(void);

//*****************************************************************************
//
//! \brief Get state of mote
//!
//! \param  _ui8State: pointer to state from mote
//!
//! \return  On success, zero is returned.
//!          On error, negative is returned.
//!
//*****************************************************************************
INT8 mote_api_get_state(UINT8 *_ui8State);

//*****************************************************************************
//
//! \brief Get event of mote
//!
//! \param  _ui32Event: pointer to event from mote
//!
//! \return  On success, zero is returned.
//!          On error, negative is returned.
//!
//*****************************************************************************
INT8 mote_api_get_event(UINT32 *_ui32Event);

//*****************************************************************************
//
//! \brief Automate to join the network
//!
//! \param  None
//!
//! \return  None
//!
//*****************************************************************************
void mote_api_autojoin(void);

//*****************************************************************************
//
//! \brief Get socket IP
//!
//! \param  _i8SocketID: pointer to socket ID
//!
//! \return  On success, zero is returned.
//!          On error, negative is returned.
//!
//*****************************************************************************
INT8 mote_api_get_socketid(INT8 *_i8SocketID);



//*****************************************************************************
//            These functions as below are for interval used
//*****************************************************************************
void mote_api_cmd_join(void);
INT8 mote_api_open_socket(UINT8 protocol);
void mote_api_bind_socket(UINT8 socketId, UINT16 port);
void mote_api_response(UINT8 command, UINT8 code);
void mote_api_set_networkid(INT16 i16Id);
INT8 mote_api_set_flag(INT8 _i8Flag);
INT16 mote_api_recvdata(UINT8 *_pui8RecvBuf, UINT8 _ui8BufSize);
INT16 mote_api_check_response(void);
INT16 mote_api_check_status(UINT16 _u16PktId);
void mote_api_receive_request(UINT8* rcvBuff, UINT8 length);
void mote_api_receive_response(UINT8* rcvBuff, UINT8 length);
INT8 mote_api_radiotest(void);
INT16 mote_api_hw_reset(void);
INT16 mote_api_get_service_info(UINT16 _u16DestAddr, UINT8 _u8Type, ServiceInfo_t *_ptInfo);
INT16 mote_api_disconnect(void);
INT16 mote_api_getp_appinfo(AppInfo_t *_ptInfo);
INT16 mote_api_getp_antgain(INT8 *_pi8Gain);
INT16 mote_api_getp_autojoin(BOOL *_pbAutoJoin);
INT16 mote_api_getp_charge(Charge_t *_ptCharge);
INT16 mote_api_getp_ipv6address(UINT8 *_pui8MacAddr);
INT16 mote_api_getp_joindutycycle(UINT8 *_pu8JoinDutyCycle);
INT16 mote_api_getp_macaddress(UINT8 *_pui8MacAddr);
INT16 mote_api_getp_shortaddress(UINT16 *_pui16moteid);
INT16 mote_api_getp_motestatus(MoteStatus_t *_ptMoteStatus);
INT16 mote_api_getp_networkid(UINT16 *_pui16Id);
INT16 mote_api_getp_otaplockout(BOOL *_pbOtap);
INT16 mote_api_getp_routingmode(BOOL *_pbMode);
INT16 mote_api_getp_time(Time_t *_ptTime);
INT16 mote_api_getp_txpower(INT8 *_pi8TxPower);
INT16 mote_api_getp_eucompliantmode(UINT8 *_pu8Mode);
INT16 mote_api_getp_eventmask(UINT32 *_pu32Event);
INT16 mote_api_getp_moteinfo(MoteInfo_t *_ptInfo);
INT16 mote_api_getp_netinfo(NetInfo_t *_ptInfo);
INT16 mote_api_setp_eventmask(UINT32 _u32Event);
INT16 mote_api_setp_antgain(INT8 _i8Gain);
INT16 mote_api_setp_autojoin(BOOL _bAutoJoin);
INT16 mote_api_setp_joindutycycle(UINT8 _u8JoinDutyCycle);
INT16 mote_api_setp_joinkey(INT8 *_pi8Key);
INT16 mote_api_setp_macaddress(UINT8 *_pui8MacAddr);
INT16 mote_api_setp_networkid(UINT16 _u16Id);
INT16 mote_api_setp_otaplockout(BOOL _bOtap);
INT16 mote_api_setp_routingmode(BOOL _bMode);
INT16 mote_api_setp_txpower(INT8 _i8TxPower);
INT16 mote_api_setp_eucompliantmode(UINT8 _u8Mode);
INT16 mote_api_setp_eventmask(UINT32 _u32Event);




//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif

