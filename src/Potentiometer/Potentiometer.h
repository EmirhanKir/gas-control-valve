#include <msp430.h>

// Function to initialize ADC Channel A5 (Pin 1.5) for the potentiometer
void pot_init();
// Function to read the current potentiometer value (returns 0-255)
char pot_Read();
