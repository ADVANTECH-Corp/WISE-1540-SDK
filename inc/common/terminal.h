
#ifndef __TERMINAL_H__
#define __TERMINAL_H__

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

int Term_iInit(void); 

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* __TERMINAL_H__ */

