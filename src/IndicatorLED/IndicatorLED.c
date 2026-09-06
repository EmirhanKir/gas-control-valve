#include "IndicatorLED.h"

void Ignition_init()
{
    // Configure Pin 5.2
    P5DIR |= BIT2;      // Set P5.2 to output direction
    P5SEL0 &= ~BIT2;    // Ensure GPIO function (00b)
    P5SEL1 &= ~BIT2;    // Ensure GPIO function (00b)

    // Clear GPIO lock to enable pins after power-on reset
    PM5CTL0 &= ~LOCKLPM5;

    P5OUT &= ~BIT2;     // Initialize LED to OFF (Active High logic)
}

void Ignition_setState(int state)
{
    // High-level logic to communicate heater status
    if (state)
    {
        P5OUT |= BIT0;  // System is ON: Set P5.0 HIGH
    }
    else
    {
        P5OUT &= ~BIT0; // System is OFF: Set P5.0 LOW
    }
}

char Ignition_Read()
{
    // Return 1 if pin is HIGH, 0 if LOW
    if (P5OUT & BIT0)
        return 1;
    else
        return 0;
}
