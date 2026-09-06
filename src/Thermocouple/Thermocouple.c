#include "Thermocouple.h"

// The raw thermocouple output was too small to read directly, so the
// MSP430's onboard Smart Analog Combo (SAC) is used as a non-inverting
// amplifier (33x gain) before the ADC reads the signal.

void thermocouple_init(void) {
    // 1. Configure SAC0 Pins: P1.1 (In+), P1.3 (Output)
    P1SEL0 |= BIT1 | BIT3;
    P1SEL1 |= BIT1 | BIT3;

    // 2. Setup SAC0 as Non-Inverting Amp (Gain 33x)
    SAC0OA |= PSEL_0 | NSEL_1;             // Pos input: P1.1, Neg: PGA
    SAC0PGA = MSEL_2 | GAIN2 | GAIN1 | GAIN0; // Non-inverting Mode
    SAC0OA |= SACEN | OAEN;                // Enable Module
}

char thermocouple_read(void) {
    ADCCTL0 &= ~ADCENC;                 // Disable ADC to switch channel
    ADCMCTL0 = ADCINCH_3 | ADCSREF_0;   // Point to P1.3 (SAC Output)
    ADCCTL0 |= ADCENC | ADCSC;          // Enable and Start
    while (!(ADCIFG & ADCIFG0));        // Wait for conversion
    return (char)(ADCMEM0 >> 2);        // 10-bit to 8-bit
}
