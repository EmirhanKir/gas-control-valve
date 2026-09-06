#include "Servo.h"

void Servo_init()
{
    // Initialize Pins
    P2DIR |= BIT1;          // set servo pin 2.1
    P2SEL0 |= BIT1;         // Set P2SEL to TB3
    P2SEL1 &= ~(BIT1);      // set starting state to zero

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Initialize Timer
    // Set period to 20ms (at 1MHz SMCLK, 20,000 counts)
    TB1CCR0 = 20000 - 1;    // defines the PWM period
    TB1CCTL2 = OUTMOD_7;    // Set Servo Pin to Reset/Set
    TB1CCR2 = 1000;         // Start at 0 degrees ("Off" position: 1ms pulse)
    TB1CTL = TBSSEL__SMCLK | MC__UP | TBCLR; // SMCLK, up mode, clear TBR
}

void Servo_setAngle(long Angle)
{
    TB1CCR2 = Angle;
}
