#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_
#include "driverlib.h"

void init_UART();
void init_sdc();
void ext_uart_crlf();
void ext_uart_transmit_string(char string[]);
void ext_uart_transmit_string2(char *string);

#endif /* PERIPHERALS_H_ */
