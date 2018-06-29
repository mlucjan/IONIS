#ifndef CLOCK_SYSTEM_H_
#define CLOCK_SYSTEM_H_

#include "driverlib.h"

//*****************************************************************************
//
//Target frequency for MCLK in kHz
//
//*****************************************************************************
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   12000

//*****************************************************************************
//
//MCLK/FLLRef Ratio
//
//*****************************************************************************
#define UCS_MCLK_FLLREF_RATIO   366

void set_clock_signals();

#endif /* CLOCK_SYSTEM_H_ */
