#include "UART.h"

void UART_BCL_Init() {
    P4SEL0 |= (BIT2 | BIT3);
    P4SEL1 &= ~(BIT2 | BIT3);

    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 6;
    UCA1MCTLW = 0x2081;
    UCA1CTLW0 &= ~UCSWRST;
    UCA1IE |= UCRXIE;
}

void UART_WIFI_Init() {
    P1SEL0 |= (BIT6 | BIT7);
    P1SEL1 &= ~(BIT6 | BIT7);

    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 6;
    UCA0MCTLW = 0x2081;
    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
}

void UART_BCL_SendCharArray(char *charArray) {
    while (*charArray != '\0') {
        while (!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *charArray++;
    }
}

void UART_WIFI_SendCharArray(char *charArray) {
    while (*charArray != '\0') {
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = *charArray++;
    }
}

#pragma vector=USCI_A1_VECTOR
__interrupt void BCL_UART_ISR(void) {
    switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) {
        case USCI_UART_UCRXIFG:
            while(!(UCA1IFG & UCTXIFG));
            UCA1TXBUF = UCA1RXBUF; // Echo character back
            break;
        default: break;
    }
}
