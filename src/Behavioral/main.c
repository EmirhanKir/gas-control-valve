// Gas Control Valve System - full behavioral implementation.
// Six-state machine (IDLE, IGNITION, STABILIZATION, HEATING, SAFETY,
// CLOSE_VALVE) driving ignition, temperature-regulated heating, and
// fail-safe shutdown on flame loss, ignition failure, or overheating.
// Reports state over UART to an ESP8266 for remote monitoring.

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include "Servo.h"
#include "Thermostat.h"
#include "Thermistor.h"
#include "Thermocouple.h"
#include "Solenoid.h"
#include "RGBLED.h"
#include "Potentiometer.h"
#include "UART.h"
#include "ADC.h" // Required for ADC_Result and channel definitions

// Global tracking for reporting
volatile uint16_t wifiReportTicks = 0;
char wifiBuffer[64];
volatile unsigned int ADC_Result; // Shared with ADC ISR

// Constants and State Definitions
#define OVERHEAT_THRESHOLD 800

typedef enum {
    IDLE,
    IGNITION,
    SAFETY,
    HEATING,
    STABILIZATION,
    CLOSE_VALVE
} SystemState;

// Global volatile variables
volatile SystemState state = IDLE;
volatile uint16_t stabilizationTimer = 0;
int tryCount = 0;
const int MAX_TRIES = 3;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop Watchdog

    // 1. Initialize Subsystems
    Solenoid_init();      // P5.4
    pot_init();            // ADC A5
    thermistor_init();     // ADC A6
    thermocouple_init();   // SAC0 and ADC A3
    Servo_init();           // P2.1 Timer Setup
    RGB_init();              // P6.0-6.2 PWM
    Thermostat_init();      // P3.0 Input setup
    UART_WIFI_Init();       // Initialize ESP8266
    UART_BCL_Init();        // Initialize Console

    // 2. Configure Timer B2 for 50ms Base Tick
    TB2CCR0 = 1638;
    TB2CTL = TBSSEL__ACLK | MC__UP | TBCLR;
    TB2CCTL0 = CCIE; // Enable Timer B2 Interrupt

    P1DIR &= ~BIT3;

    PM5CTL0 &= ~LOCKLPM5; // Activate GPIO
    __enable_interrupt();  // Enable interrupts globally

    while (1) {
        // Refresh sensor data (These now use the 12-bit ADC result)
        int16_t currentTemp = thermistor_read();
        char flameState = thermocouple_read();
        char callForHeat = (P3IN & BIT0) ? 1 : 0; // Read Thermostat pin P3.0
        char potValue = pot_Read();

        // 30-Second Automatic Reporting
        if (wifiReportTicks >= 600) {
            wifiReportTicks = 0;
            sprintf(wifiBuffer, "%d,%d,%d\r\n", currentTemp, flameState, (int)state);
            UART_WIFI_SendCharArray(wifiBuffer);
        }

        switch (state) {
            case IDLE:
                RGB_setColor(255, 0, 0); // Solid Red
                Solenoid_off();
                Servo_setAngle(1000);     // Closed position
                if (callForHeat == 1 && currentTemp < OVERHEAT_THRESHOLD) {
                    state = IGNITION;
                    tryCount = 0;
                } else if (currentTemp > OVERHEAT_THRESHOLD) {
                    state = SAFETY;
                }
                break;

            case IGNITION:
                RGB_setColor(0, 255, 0); // Solid Green
                Solenoid_on();            // Open gas / ignite
                __delay_cycles(1000000);
                if (thermocouple_read() == 1) {
                    Servo_setAngle(2000); // Open Main Valve
                    __delay_cycles(1000000);
                    state = STABILIZATION;
                    stabilizationTimer = 0;
                } else {
                    __delay_cycles(500000);
                    if (++tryCount >= MAX_TRIES) state = CLOSE_VALVE;
                }
                break;

            case HEATING:
                if (currentTemp < (potValue - 2)) {
                    Servo_setAngle(2000);
                } else if (currentTemp > (potValue + 2)) {
                    Servo_setAngle(1000);
                    state = STABILIZATION;
                }
                if (callForHeat == 0 || flameState == 0) state = CLOSE_VALVE;
                break;

            case STABILIZATION:
                if (stabilizationTimer++ > 40) { // ~2s wait
                    state = HEATING;
                }
                if (callForHeat == 0) state = CLOSE_VALVE;
                break;

            case SAFETY:
                RGB_setColor(0, 0, 255); // Solid Blue
                Solenoid_off();
                Servo_setAngle(1000);
                if (currentTemp < (OVERHEAT_THRESHOLD - 50)) state = IDLE;
                break;

            case CLOSE_VALVE:
                Solenoid_off();
                Servo_setAngle(1000);
                __delay_cycles(1000000);
                state = IDLE;
                break;
        }

        __delay_cycles(50000); // 50ms loop stability
    }
}

// --- INTERRUPT SERVICE ROUTINES ---

// Timer B2 ISR: Handles Ticks and RGB blink pattern per state
#pragma vector=TIMER2_B0_VECTOR
__interrupt void Timer_B2_CCR0_ISR(void) {
    wifiReportTicks++;

    if (state == HEATING) {
        TB3CCR1 = (TB3CCR1 == 0) ? 255 : 0; // Toggle Red
        TB3CCR2 = 0; TB3CCR3 = 0;
    } else if (state == STABILIZATION) {
        TB3CCR2 = (TB3CCR2 == 0) ? 255 : 0; // Toggle Green
        TB3CCR1 = 0; TB3CCR3 = 0;
    } else if (state == CLOSE_VALVE) {
        TB3CCR3 = (TB3CCR3 == 0) ? 255 : 0; // Toggle Blue
        TB3CCR1 = 0; TB3CCR2 = 0;
    }
}

// ADC ISR: Required since ADCIE is enabled in ADC.c
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void) {
    switch (__even_in_range(ADCIV, ADCIV_ADCIFG)) {
        case ADCIV_ADCIFG:
            ADC_Result = ADCMEM0; // Store 12-bit result
            __bic_SR_register_on_exit(LPM0_bits); // Wake up CPU
            break;
        default: break;
    }
}

// UART BCL ISR: Handles Echo for Console
#pragma vector=USCI_A1_VECTOR
__interrupt void BCL_UART_ISR(void) {
    switch (__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) {
        case USCI_UART_UCRXIFG:
            while (!(UCA1IFG & UCTXIFG)); // Wait for TX buffer
            UCA1TXBUF = UCA1RXBUF;         // Echo character
            break;
        default: break;
    }
}

// UART WIFI ISR: Mandatory to prevent trap if WIFI UART sends data
#pragma vector=USCI_A0_VECTOR
__interrupt void WIFI_UART_ISR(void) {
    // Read the buffer to clear the interrupt flag and prevent hanging
    volatile char dummy = UCA0RXBUF;
}
