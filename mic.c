#include "mic.h"

uint32_t mic_convert_once(){
    SD24_B_startConverterConversion(SD24_BASE,
                                    SD24_B_CONVERTER_0); // Set bit to start conversion

    // Poll interrupt flag for channel 0
    while( SD24_B_getInterruptStatus(SD24_BASE,
                                     SD24_B_CONVERTER_0,
                                     SD24_B_CONVERTER_INTERRUPT) == 0 );

                // Save CH0 results (clears IFG)
    return SD24_B_getResults(SD24_BASE, SD24_B_CONVERTER_0);
}
