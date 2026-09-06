// Component-level integration test: cycles the servo and solenoid through
// an open/close sequence, reads all three analog sensors, and drives the
// RGB LED off the potentiometer, ahead of wiring in the full state machine.

#include <msp430.h>
#include "ADC.h"
#include "Potentiometer.h"
#include "Thermistor.h"
#include "Thermocouple.h"
#include "RGBLED.h"
#include "Thermostat.h"
#include "Servo.h"
#include "Solenoid.h"

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog Timer

    // 1. Initialize Peripheral Modules
    ADC_init();
    RGB_init();
    thermistor_init();
    thermocouple_init();
    Thermostat_init();  // P3.0 Input -> P5.0 Output
    Servo_init();        // P2.1 Timer Setup
    Solenoid_init();     // P5.4 Output Setup

    // 2. Clear GPIO lock to enable pins (P3.0, P5.0, P5.4, etc.)
    PM5CTL0 &= ~LOCKLPM5;

    // 3. Variables for sensor data
    uint16_t potVal = 0;
    uint16_t thermVal = 0;
    uint16_t tcVal = 0;

    while (1) {
        // 4. Read Sensors
        potVal = ADC_readPot();
        thermVal = ADC_readThermistor();
        tcVal = ADC_readThermocouple();

        // 5. Thermostat Logic
        // Routes Pin 3.0 state to Pin 5.0 Red LED
        Thermostat_update(thermVal);

        // 6. Actuator Cycle (Servo & Solenoid)
        // --- State: OFF / DOWN ---
        Solenoid_on();
        Servo_setAngle(1000);   // Move Servo to 0 degrees
        __delay_cycles(2000000); // 2-second delay

        // --- State: ON / UP ---
        Solenoid_off();
        Servo_setAngle(2000);   // Move Servo to 180 degrees
        __delay_cycles(2000000); // 2-second delay

        // 7. Visual Feedback
        // Potentiometer controls Green channel of RGB LED
        RGB_setColor(0, (uint8_t)(potVal >> 4), 0);

        // 8. Stability Delay
        __delay_cycles(100000);
    }
}
