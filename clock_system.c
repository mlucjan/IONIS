#include "clock_system.h"

void set_clock_signals(){

    //Set VCore = 1 for 12MHz clock
    PMM_setVCore(PMM_CORE_LEVEL_1);

    //Set DCO FLL reference = REFO
    UCS_initClockSignal(
            UCS_FLLREF,
            UCS_REFOCLK_SELECT,
            UCS_CLOCK_DIVIDER_1
    );

    //Set Ratio and Desired MCLK Frequency  and initialize DCO
    UCS_initFLLSettle(
            UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
            UCS_MCLK_FLLREF_RATIO
    );

}
