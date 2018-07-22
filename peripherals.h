#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_
#include "driverlib.h"

//*****************************************************************************
//
//Specify desired frequency of SPI communication
//
//*****************************************************************************
#define SPICLK 500000


void init_uart();
void init_sdc();
void ext_uart_crlf();
void ext_uart_transmit_string();

#endif /* PERIPHERALS_H_ */
