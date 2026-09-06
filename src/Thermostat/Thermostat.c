#include "Thermostat.h"

/**
 * Configures the hardware routing:
 * P3.0 = Input (call for heat)
 * P5.0 = Output (status LED)
 */
void Thermostat_init(void) {
    // Configure P5.0 as Output (LED)
    P5DIR |= BIT0;
    P5OUT &= ~BIT0;

    // Configure P3.0 as Input
    P3DIR &= ~BIT0;  // Set P3.0 to input direction
    P3REN |= BIT0;   // Enable internal resistor
    P3OUT &= ~BIT0;  // Set to Pull-Down (reads 0 unless 3.3V is applied)
}

/**
 * Transfers the state of P3.0 to P5.0.
 * If currentTemp is used as a safety override, that logic can be added here.
 */
void Thermostat_update(uint16_t currentTemp) {
    // If Pin 3.0 is HIGH, set Pin 5.0 HIGH. Otherwise, set it LOW.
    if (P3IN & BIT0) {
        P5OUT |= BIT0;
    } else {
        P5OUT &= ~BIT0;
    }
}
