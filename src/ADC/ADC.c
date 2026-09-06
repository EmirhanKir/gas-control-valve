// Interrupt-driven ADC driver used by the full behavioral state machine.
// Each read triggers a conversion, then sleeps in LPM0 until the ISR
// stores the result and wakes the CPU, instead of busy-polling a flag.

#include "ADC.h"

volatile unsigned int ADC_Result;

void ADC_init(void) {
    // 1. Configure ADC Control Registers
    // ADCON=1 (Turn on), ADCSHT_2 (16 clocks), ADCSHP (sampling timer)
    ADCCTL0 |= ADCSHT_2 | ADCON;
    ADCCTL1 |= ADCSHP;
    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;     // 12-bit conversion
    ADCIE |= ADCIE0;         // Enable complete interrupt

    // 2. Configure Port 1 Selection for Analog Mode
    // P1.1=OA0+, P1.3=A3, P1.5=A5, P1.6=A6
    P1SEL0 |= BIT1 | BIT3 | BIT5 | BIT6;
    P1SEL1 |= BIT1 | BIT3 | BIT5 | BIT6;

    // 3. Configure Port 5 for Solenoid Output
    // P5.4 = Solenoid Control
    P5DIR |= BIT4;
    P5OUT &= ~BIT4;           // Ensure Solenoid starts CLOSED (OFF)

    // 4. Prevent Leakage on unused pins
    // Terminate unused Port 1 pins as outputs to ground
    P1DIR |= (BIT0 | BIT2 | BIT4 | BIT7);
    P1OUT &= ~(BIT0 | BIT2 | BIT4 | BIT7);

    // Clear GPIO lock to enable pins after power-on reset
    PM5CTL0 &= ~LOCKLPM5;
}

// Private helper to trigger a sample and enter low-power mode
uint16_t ADC_readChannel(uint16_t channel) {
    ADCCTL0 &= ~ADCENC;             // Disable ADC to switch channel
    ADCMCTL0 = channel;             // Select channel (Vref = AVCC/AVSS)
    ADCCTL0 |= ADCENC | ADCSC;      // Enable and Start conversion

    // Enter Low Power Mode 0 and wait for the ISR to wake the CPU
    __bis_SR_register(LPM0_bits | GIE);
    return ADC_Result;
}

uint16_t ADC_readPot(void) {
    return ADC_readChannel(POT_CHANNEL); // Channel A5 (P1.5)
}

unsigned int ADC_readThermistor(void) {
    return ADC_readChannel(THERMISTOR_CHANNEL); // Channel A6 (P1.6)
}

unsigned int ADC_readThermocouple(void) {
    return ADC_readChannel(THERMOCOUPLE_CHANNEL); // Channel A3 (P1.3)
}

// ADC interrupt service routine
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void) {
    switch (__even_in_range(ADCIV, ADCIV_ADCIFG)) {
        case ADCIV_ADCIFG:
            ADC_Result = ADCMEM0;               // Store 12-bit result
            __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
            break;
        default: break;
    }
}
