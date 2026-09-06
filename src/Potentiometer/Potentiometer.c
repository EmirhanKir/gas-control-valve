#include "Potentiometer.h"

void pot_init()
{
    // Configure Pin 1.5 for Analog Input (A5)
    P1SEL0 |= BIT5;
    P1SEL1 |= BIT5;

    // Clear GPIO lock to enable pins after power-on reset
    PM5CTL0 &= ~LOCKLPM5;

    // Configure ADC12 Control Registers
    // ADCON: Turn ADC ON
    // ADCSHT_2: 16 ADC clock cycles for sample-and-hold time
    ADCCTL0 |= ADCON | ADCSHT_2;

    // ADCSHP: Use internal sampling timer
    // ADCSSEL_2: Use SMCLK as the clock source
    ADCCTL1 |= ADCSHP | ADCSSEL_2;

    // ADCRES_1: Set resolution to 10-bit (sufficient for returning a char)
    ADCCTL2 |= ADCRES_1;

    // ADCMCTL0: Select Channel A5 (Potentiometer)
    // ADCSREF_0: Use internal VCC/VSS as references
    ADCMCTL0 |= ADCINCH_5 | ADCSREF_0;
}

char pot_Read()
{
    // Enable conversion and start sampling
    ADCCTL0 |= ADCENC | ADCSC;

    // Wait for the conversion to complete (poll the ADCIFG0 flag)
    while (!(ADCIFG & ADCIFG0));

    // Read the result from ADCMEM0
    // Since ADCMEM0 is 10-bit (0-1023) and we return a char (0-255),
    // we shift the result right by 2 bits.
    int rawValue = ADCMEM0;
    char mappedValue = (char)(rawValue >> 2);
    return mappedValue;
}
