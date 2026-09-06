// Standalone test harness for the UART console link.
// Streams thermistor and potentiometer readings out over UART every ~30s
// so the serial monitor can be checked before wiring into the full
// behavioral state machine.

#include <msp430.h>
#include <stdio.h>
#include "UART.h"
#include "ADC.h"

void initClock(void);

// Global Variables
int currentState = 1;
char buffer[64];

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop Watchdog
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO lock

    initClock();                // Set stable 1.048MHz clock
    ADC_init();                 // Initialize ADC for P1.5
    UART_BCL_Init();            // Initialize Console

    __enable_interrupt();       // Enable global interrupts

    while (1) {
        uint16_t currentTemp = ADC_readThermistor();
        uint16_t potVal = ADC_readPot();

        // Format: "temp,flame,state\n"
        // Example output: "72,128,1\n"
        sprintf(buffer, "%d,%d,%d\r\n", (int)currentTemp, (int)potVal, currentState);

        // Send to Serial Monitor
        UART_BCL_SendCharArray(buffer);

        // 30 second delay loop
        int i;
        for (i = 0; i < 30; i++) {
            __delay_cycles(1000000);
        }
    }
}

void initClock(void) {
    __bis_SR_register(SCG0);
    CSCTL0 = 0;
    CSCTL1 &= ~(DCORSEL_7);
    CSCTL1 |= DCORSEL_0;
    CSCTL3 = SELREF__REFOCLK;
    CSCTL2 = FLLD_0 + 30;
    __delay_cycles(3);
    __bic_SR_register(SCG0);

    while (1) {
        CSCTL7 &= ~(FLLUNLOCK0 | FLLUNLOCK1);
        SFRIFG1 &= ~OFIFG;
        __delay_cycles(100);
        if (!(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))) break;
    }

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}
