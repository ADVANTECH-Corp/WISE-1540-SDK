
#ifndef __MOTE_COMMAND_SET_CONSOLE__H__
#define __MOTE_COMMAND_SET_CONSOLE__H__

//*****************************************************************************
//                  Included files
//*****************************************************************************
#include "mbed.h"
#include "common/console.h"



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

//*****************************************************************************
//                  MACROs and needed structures
//*****************************************************************************
#define RETERR_MSG_IF_TRUE(cond, err_code, string, ...) \
            do {\
                    if(cond) { \
                        MSG_print(string, ## __VA_ARGS__); \
                        return err_code; \
                    } \
            } while (0);

#define INVALID_MSG         "Invalid value"
#define INVALID_PARA_MSG    "Invalid parameter"
#define COMPLETE_MSG        "OK"
#define FAILED_MSG          "Failed"


enum EParaSupported{
    ePara_OnlySet,
    ePara_OnlyGet,
    ePara_Both
};

#define MOTE_COMMAND_SET \
        X(eCmdSet_mset, "mset", Cmd_iSetParaCmd, \
	                                    "mset [parameter] [value] \n\r" \
	                                    " Ex. mset autojoin 1 \n\r"), \
	    X(eCmdSet_mget, "mget", Cmd_iGetParaCmd, \
	                                    "mget [parameter] \n\r" \
	                                    " Ex. mget autojoin \n\r"), \
	    X(eCmdSet_reset, "reset", Cmd_iReset, \
	                                    "reset\n\r" \
	                                    " Ex. reset \n\r"), \
	    X(eCmdSet_disconnect, "disconnect", Cmd_iDisconnect, \
	                                    "disconnect \n\r" \
	                                    " Ex. disconnect \n\r"), \
        X(eCmdSet_radiotest, "radiotest", Cmd_iRadioTest, \
	                                    "radiotest \n\r" \
	                                    " Ex. radiotest \n\r"), \
	    X(eCmdSet_Total, NULL, NULL, NULL)

        /*	                                    
	    X(eCmdSet_getservinfo, "getservinfo", Cmd_iGetServiceInfo, \
	                                    "getservinfo [mote id] [service type] \n\r" \
	                                    " mote id: 0 - 65535 (decimal) \n\r" \
	                                    " service type: bandwidth is 1, latency is 2 \n\r" \
	                                    " Ex. getservinfo 2 1 \n\r"), \
	    */

#define MOTE_SET_GET_PARAMETER_SET \
        X(ePara_autojoin, "autojoin", SGetP_iAutojoin, ePara_Both, \
	                                    "value: 1 or 0 (decimal) \n\r"), \
	    X(ePara_joindc, "joindc", SGetP_iDutyCycle, ePara_Both, \
	                                    "value: 0 ~ 255 (decimal) \n\r"), \
	    X(ePara_antGain, "antGain", SGetP_iAntGain, ePara_Both, \
	                                    "value: -127 ~ 127 (decimal) \n\r"), \
	    X(ePara_appinfo, "appinfo", SGetP_iGetInfo, ePara_OnlyGet, \
	                                    "value: string \n\r"), \
	    X(ePara_ipv6addr, "ipv6addr", SGetP_iIpv6Address, ePara_OnlyGet, \
	                                    "value: string \n\r"), \
	    X(ePara_macaddr, "macaddr", SGetP_iMacAddress, ePara_Both, \
	                                    "value: 01:23:45:67:89:ab:cd:ef (hex) \n\r"), \
	    X(ePara_rtmode, "rtmode", SGetP_iRoutingMode, ePara_Both, \
	                                    "value: 1:routing enabled or 0:routing disabled (decimal) \n\r"), \
	    X(ePara_txpwr, "txpwr", SGetP_iTxPower, ePara_Both, \
	                                    "value: 0 or 8 (decimal) \n\r"), \
	    X(ePara_netid, "netid", SGetP_iNetId, ePara_Both, \
	                                    "value: 0 ~ 65535 (decimal) \n\r"), \
	    X(ePara_time, "time", SGetP_iTime, ePara_OnlyGet, \
	                                    "value: string \n\r"), \
	    X(ePara_jkey, "jkey", SGetP_iJoinKey, ePara_OnlySet, \
	                                    "value: 4a4f494e414456414e54454348494f54 (hex) \n\r"), \
	    X(ePara_compMode, "compMode", SGetP_iCompMode, ePara_Both, \
	                                    "value: 1 = ON, 0 = OFF (decimal) \n\r"), \
	    X(ePara_emask, "emask", SGetP_iEventMask, ePara_Both, \
	                                    "value: 0xffffffff (hex) \n\r"), \
	    X(ePara_minfo, "minfo", SGetP_iMoteInfo, ePara_OnlyGet, \
	                                    "value: string \n\r"), \
	    X(ePara_mstatus, "mstatus", SGetP_iMoteStatus, ePara_OnlyGet, \
	                                    "value: string \n\r"), \
        X(ePara_ninfo, "ninfo", SGetP_iNetInfo, ePara_OnlyGet, \
	                                    "value: string \n\r"), \
	    X(ePara_Total, NULL, NULL, NULL, NULL)

#define X(cmd, cmd_name, func, info) cmd
enum EMoteCommandSet{
    MOTE_COMMAND_SET
};
#undef X

#define X(cmd, para_name, func, method, info) cmd
enum ESetGetParameter{
    MOTE_SET_GET_PARAMETER_SET
};
#undef X

enum EAction{
    eAct_Set,
    eAct_Get
};

//*****************************************************************************
//                  Functions prototypes
//*****************************************************************************
int mote_command_set_init(void);



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* __MOTE_COMMAND_SET_CONSOLE__H__ */

