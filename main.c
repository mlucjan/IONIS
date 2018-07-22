#include <stdlib.h>
#include "clock_system.h"
#include "mic.h"

unsigned char timerRunning = 0x00; //flaga do oznaczania cykli zbierania próbek
uint32_t currResult, absMinResult, absMaxResult;
uint32_t maxResult = 0;
uint32_t minResult = 0x00FFFFFF;
uint16_t timerOverflowCounter=0;
uint16_t maxTimerOverflows = 10; //12MHz zegar + divider(2) -> timer przepełnia się w ~11ms
char resultText[8];
int i;

// czyszczenie tablicy resultText
void clearResultText(){
    for(i=0; i< sizeof(resultText); i++){
        resultText[i]='\0';
    }
}
void main (void)
{

    // stop watchdog
    WDT_A_hold(WDT_A_BASE);

    set_clock_signals();
    init_uart();
    init_sdc();

    //Start timer in continuous mode sourced by SMCLK
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
    Timer_A_initContinuousModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_2;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true;

    timerRunning = 0x01;
    Timer_A_initContinuousMode(TIMER_A1_BASE, &param);
    SD24_B_startConverterConversion(SD24_BASE, 0);

    while(1){
        if(timerRunning == 0x00){

            SD24_B_stopConverterConversion(SD24_BASE, 0);

            //koncepcja z obliczaniem większego modułu (teraz nie działa, bo wynik przechowywany w typie unsigned)
//            absMinResult = abs(minResult);
//            absMaxResult = abs(maxResult);
//
//            if(absMinResult >= absMaxResult){
//                ltoa(absMinResult, resultString);
//                for(i=0; i<sizeof(resultString); i++){
//                    if(resultString[i] != '\0'){
//                        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, resultString[i]);
//                        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
//                            ;
//                        }
//                    }
//                }
//                ext_uart_crlf();
//            }
//            else{
//                ltoa(absMaxResult, resultString);
//                for(i=0; i<sizeof(resultString); i++){
//                    if(resultString[i] != '\0'){
//                        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, resultString[i]);
//                        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
//                            ;
//                        }
//                    }
//                }
//                ext_uart_crlf();
//            }



            ltoa(minResult, resultText);
            for(i=0; i<sizeof(resultText); i++){
                if(resultText[i] != '\0'){
                    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, resultText[i]);
                    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
                        ;
                    }
                }
            }
            ext_uart_crlf();
            clearResultText();
            ltoa(maxResult, resultText);
            for(i=0; i<sizeof(resultText); i++){
                if(resultText[i] != '\0'){
                    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, resultText[i]);
                    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
                        ;
                    }
                }
            }
            ext_uart_crlf();
            clearResultText();

            //ext_uart_transmit_string(resultString);
            timerRunning = 0x01;
            minResult = 0x00FFFFFF;
            maxResult = 0;
            Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
            SD24_B_startConverterConversion(SD24_BASE, 0);

        }

    }

}

//******************************************************************************
//
//This is the USCI_A1 interrupt vector service routine.
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
//This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR (void)
{
    switch (__even_in_range(UCA0IV,4)){
        //Vector 2 - RXIFG
        case 2:
            //USCI_A0 TX buffer ready?

            break;
        default: break;
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
            if(timerOverflowCounter >= maxTimerOverflows){ // timer do odmierzania długości cyklu próbkowania
                timerOverflowCounter = 0;
                timerRunning = 0x00;
                Timer_A_stop(TIMER_A1_BASE);
                Timer_A_clear(TIMER_A1_BASE);
            }
            break;
        default: break;
    }
}
//******************************************************************************
//
//This is the SD24_B interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD24B_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(SD24B_VECTOR)))
#endif
void SD24BISR(void)
{
    switch (SD24BIV)
    {
        case SD24BIV_SD24OVIFG:             // SD24MEM Overflow
            break;
        case SD24BIV_SD24TRGIFG:            // SD24 Trigger IFG
            break;
        case SD24BIV_SD24IFG0:              // SD24MEM0 IFG
            // Save CH0 results (clears IFG)
            currResult = SD24_B_getResults(SD24_BASE, SD24_B_CONVERTER_0);
            if(currResult > maxResult){ //śledzenie wartości max i min
                maxResult = currResult;
            }
            else if(currResult < minResult){
                minResult = currResult;
            }
            break;
        case SD24BIV_SD24IFG1:              // SD24MEM1 IFG
            break;
        case SD24BIV_SD24IFG2:              // SD24MEM2 IFG
            break;
        default: break;
    }
}
