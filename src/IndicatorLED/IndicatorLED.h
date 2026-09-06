#include <msp430.h>

// Initialize Port 5.2 for GPIO output
void Ignition_init();
// Set system status: 1 for ON, 0 for OFF
void Ignition_setState(int state);
// Read current hardware status of the LED
char Ignition_Read();
