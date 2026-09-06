// Polling variant of the ADC driver, used during early UART bring-up testing
// (paired with UARTmain_test.c) before switching to the interrupt-driven
// version in ADC.c for the full behavioral build.

#include "ADC.h"

volatile unsigned int ADC_Result; // Global to store ISR results

void ADC_init(void) {
    P1SEL0 |= BIT3 | BIT5 | BIT6;
    P1SEL1 |= BIT3 | BIT5 | BIT6;

    ADCCTL0 |= ADCSHT_2 | ADCON;  // 16 clocks, ADC ON
    ADCCTL1 |= ADCSHP;            // Pulse sample mode
    ADCCTL2 |= ADCRES_2;          // 12-bit resolution
    ADCIE |= ADCIE0;              // Enable interrupt
}

uint16_t ADC_readChannel(uint16_t channel) {
    ADCCTL0 &= ~ADCENC;                  // Disable to change channel
    ADCMCTL0 = channel | ADCSREF_0;      // Set target channel
    ADCCTL0 |= ADCENC | ADCSC;           // Start conversion
    __bis_SR_register(LPM0_bits | GIE);  // Sleep until ISR finishes
    return ADC_Result;
}

uint16_t ADC_readPot(void) {
    return ADC_readChannel(POT_CHANNEL); // A5
}

unsigned int ADC_readThermistor(void) {
    return ADC_readChannel(THERMISTOR_CHANNEL); // A6
}

unsigned int ADC_readThermocouple(void) {
    return ADC_readChannel(THERMOCOUPLE_CHANNEL); // A3
}

#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void) {
    switch (__even_in_range(ADCIV, ADCIV_ADCIFG)) {
        case ADCIV_ADCIFG:
            ADC_Result = ADCMEM0; // Store result
            __bic_SR_register_on_exit(LPM0_bits); // Wake up
            break;
    }
}
