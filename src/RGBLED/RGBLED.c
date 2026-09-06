#include <msp430.h>
#include "RGBLED.h"

void RGB_init()
{
    // Initialize Pins
    P6DIR |= BIT0 | BIT1 | BIT2;         // Set RGB Pins 6.0 - 6.2
    P6SEL0 |= BIT0 | BIT1 | BIT2;        // Set P6SEL to TB3
    P6SEL1 &= ~(BIT0 | BIT1 | BIT2);     // set starting state to zero

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Initialize Timer
    TB3CCR0 = 1000 - 1;      // Set period of PWM to 1ms
    TB3CCTL1 = OUTMOD_3;     // Set Red Pin to Reset/Set
    TB3CCR1 = 0;             // Initialize Red to OFF
    TB3CCTL2 = OUTMOD_3;     // Set Green Pin to Reset/Set
    TB3CCR3 = 0;             // Initialize Green to OFF
    TB3CCTL3 = OUTMOD_3;     // Set Blue Pin to Reset/Set
    TB3CCR3 = 0;             // Initialize Blue to OFF
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR; // SMCLK, up mode, clear TBR
}

void RGB_setColor(char Red, char Green, char Blue)
{
    short redValue = Red << 2;   // Multiply Red by 4
    if (redValue > 999)
    {
        redValue = 999;
    }

    short greenValue = Green << 2; // Multiply Green by 4
    if (greenValue > 999)
    {
        greenValue = 999;
    }

    short blueValue = Blue << 2;   // Multiply Blue by 4
    if (blueValue > 999)
    {
        blueValue = 999;
    }

    TB3CCR1 = redValue;
    TB3CCR2 = greenValue;
    TB3CCR3 = blueValue;
}
