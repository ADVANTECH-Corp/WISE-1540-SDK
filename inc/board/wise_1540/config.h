
#ifndef _WISE_1540_H_
#define _WISE_1540_H_
/*
#include <msp430.h>
#include "driverlib.h"
#include "mcu/f5419a/system.h"
#include "mcu/common/flash.h"
#include "board/wise_1540/led.h"
*/

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

#define BOARD_MODELNAME    "WISE-1540"
#define BOARD_HOSTNAME     "WISE-1540_HN"

#if !defined(BOARD_SW_VERSION)
#define BOARD_SW_VERSION   "1.0.00"
#endif
#if !defined(BOARD_HW_VERSION)
#define BOARD_HW_VERSION   "1.0.00"
#endif

#define MODELNAME                   "WISE-1540"
#define HOSTNAME                    "SensorHub"

#define TEMPERATURE_HOSTNAME        "Temperature"
#define TEMPERATURE_MODELNAME       "Hdc1050"

#define HUMIDITY_HOSTNAME     	    "Humidity"
#define HUMIDITY_MODELNAME    	    "Hdc1050"

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* _WISE_1540_H_ */

