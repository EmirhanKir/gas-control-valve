#include "Thermistor.h"

// Note: original PCB routing put an accidental trace on P1.7 shared with the
// thermistor line, which later conflicted with UART. P1.7 is held as an input
// here as a workaround; see docs/PCB_Design_Considerations.pdf for the fix
// planned for the next board revision.

void thermistor_init(void) {
    // Configure P1.6 for ADC Channel A6
    P1SEL0 |= BIT6;
    P1SEL1 |= BIT6;
    P1DIR &= ~(BIT7); // Set as input due to error on PCB
}

char thermistor_read(void) {
    ADCCTL0 &= ~ADCENC;                 // Disable ADC to switch channel
    ADCMCTL0 = ADCINCH_6 | ADCSREF_0;   // Point to P1.6
    ADCCTL0 |= ADCENC | ADCSC;          // Enable and Start
    while (!(ADCIFG & ADCIFG0));        // Wait for conversion
    return (char)(ADCMEM0 >> 2);        // 10-bit to 8-bit
}
