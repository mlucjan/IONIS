#include "clock_system.h"
#include "mic.h"

unsigned char micFlag = 0x01;
uint32_t currResult, maxResult;
uint16_t timerOverflowCounter=0;
uint16_t maxTimerOverflows = 1000;
char resultString[8];
int i;

void main (void)
{

    // stop watchdog
    WDT_A_hold(WDT_A_BASE);

    set_clock_signals();
    init_UART();
    init_sdc();

    //Start timer in continuous mode sourced by SMCLK
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
    Timer_A_initContinuousModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_2;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true;
    Timer_A_initContinuousMode(TIMER_A1_BASE, &param);

    while(1){
        while(micFlag == 0x01){
            currResult = mic_convert_once();

            if(currResult > maxResult){
                maxResult = currResult;
            }

            ltoa(currResult, resultString);
            //ext_uart_transmit_string(resultString);
            for(i=0; i<sizeof(resultString); i++){
                if(resultString[i] != '\x00'){
                    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, resultString[i]);
                    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
                        ;
                    }
                }
            }
            ext_uart_crlf();
        }
        __delay_cycles(60000000);
        micFlag = 0x01;
        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
    }

}

//******************************************************************************
//
//This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
void EUSCI_A1_ISR(void)
{
  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG: break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}

//******************************************************************************
//
//This is the TIMER1_A3 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER1_A1_VECTOR)))
#endif
void TIMER1_A1_ISR (void)
{
    //Any access, read or write, of the TAIV register automatically resets the
    //highest "pending" interrupt flag
    switch ( __even_in_range(TA1IV,14) ){
        case  0: break;                          //No interrupt
        case  2: break;                          //CCR1 not used
        case  4: break;                          //CCR2 not used
        case  6: break;                          //CCR3 not used
        case  8: break;                          //CCR4 not used
        case 10: break;                          //CCR5 not used
        case 12: break;                          //CCR6 not used
        case 14:
                                                    // overflow
            timerOverflowCounter++;
            if(timerOverflowCounter >= maxTimerOverflows){
                timerOverflowCounter = 0;
                micFlag = 0x00;
                Timer_A_stop(TIMER_A1_BASE);
            }
            break;
        default: break;
    }
}
