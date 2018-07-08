#include "peripherals.h"

void init_uart(){
    //Configure UART pins (UCA1TXD/UCA1SIMO, UCA1RXD/UCA1SOMI)
    //Set P1.4 and P1.5 as Module Function Input
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN4 + GPIO_PIN5
    );

    // Configure UART
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 6;
    param.firstModReg = 8;
    param.secondModReg = 0;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A1_BASE, &param)) {
        return;
    }

    EUSCI_A_UART_enable(EUSCI_A1_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE,
                                EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable USCI_A1 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE,
                                 EUSCI_A_UART_RECEIVE_INTERRUPT);         // Enable interrupt

    __enable_interrupt();
}

void init_sdc(){
    SD24_B_initParam initParam = {0};
    initParam.clockSourceSelect = SD24_B_CLOCKSOURCE_SMCLK;
    initParam.clockPreDivider = SD24_B_PRECLOCKDIVIDER_1;
    initParam.clockDivider = SD24_B_CLOCKDIVIDER_1;
    initParam.referenceSelect = SD24_B_REF_INTERNAL;
    SD24_B_init(SD24_BASE, &initParam);

    SD24_B_initConverterParam initConverterParam = {0};
    initConverterParam.converter = SD24_B_CONVERTER_0;
    initConverterParam.alignment = SD24_B_ALIGN_RIGHT;
    initConverterParam.startSelect = SD24_B_CONVERSION_SELECT_SD24SC;
    initConverterParam.conversionMode = SD24_B_SINGLE_MODE;
    SD24_B_initConverter(SD24_BASE, &initConverterParam);

    __delay_cycles(0x3600);  // Delay for 1.5V REF startup

    SD24_B_setOversampling(SD24_BASE, SD24_B_CONVERTER_0, SD24_B_OVERSAMPLE_256);
    //SD24_B_setGain(SD24_BASE, SD24_B_CONVERTER_0, SD24_B_GAIN_1);

}

/*void init_spi(){
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P2,
            GPIO_PIN2 + GPIO_PIN3 + GPIO_PIN5 + GPIO_PIN6
    );

    //Initialize Master
        USCI_A_SPI_initMasterParam param = {0};
        param.selectClockSource = USCI_A_SPI_CLOCKSOURCE_SMCLK;
        param.clockSourceFrequency = UCS_getSMCLK();
        param.desiredSpiClock = SPICLK;
        param.msbFirst = USCI_A_SPI_MSB_FIRST;
        param.clockPhase = USCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
        param.clockPolarity = USCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
        returnValue =  USCI_A_SPI_initMaster(USCI_A0_BASE, &param);

        if (STATUS_FAIL == returnValue){
            return;
        }

        //Enable SPI module
        USCI_A_SPI_enable(USCI_A0_BASE);

        //Enable Receive interrupt
        USCI_A_SPI_clearInterrupt(USCI_A0_BASE,
            USCI_A_SPI_RECEIVE_INTERRUPT);
        USCI_A_SPI_enableInterrupt(USCI_A0_BASE,
            USCI_A_SPI_RECEIVE_INTERRUPT);
}*/

void ext_uart_crlf(){

    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, 0x0d); //CR

    while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
        ;
    }

    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, 0x0a); //LF
}

/*
 * !!!!!! Zaszumia strasznie przebieg z mikrofonu
 *
 */
void ext_uart_transmit_string(char string[]){
    int i;
    for(i=0; i<sizeof(string); i++){
        if(string[i] != '\x00'){
            EUSCI_A_UART_transmitData(EUSCI_A1_BASE, string[i]);
            while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
                ;
            }
        }
    }

    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, 0x0d); //CR

        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
            ;
        }

        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, 0x0a); //LF
}
