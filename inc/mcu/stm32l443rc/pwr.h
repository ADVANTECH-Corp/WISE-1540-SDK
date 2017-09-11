#ifndef __PWR_H__
#define __PWR_H__

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
    ePwrModeNo = 0,
    ePwrModeFullSpeed = 1,
    ePwrModeSleep = 2,
    ePwrModeDeepSleep = 3,
    ePwrModePowerDown = 4,
    ePwrModeDeepPowerDown = 5,
    ePwrModeReducedPower = 6,
    ePwrModePeripheralPowerDown = 7,
    ePwrModePhyPowerDown = 8,
    ePwrModeReset = 9,
    ePwrModeUnKnown = 10
} ePwrMode;

typedef enum
{
    ePwrOnOff_No = 1,
    ePwrOnOff_On = 2,
    ePwrOnOff_Off = 3,
    ePwrOnOff_UnKnown = 4
} ePwrOnOff;

// ---------------- tagPowerConfig  -------------------------
typedef struct tagPowerConfig
{
  ePwrMode pwr_mode;    // idel, sleep, full
  ePwrOnOff pwr_onoff;
} PowerConfig_t, *PowerConfig_tp;

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // End of __PWR_H__
