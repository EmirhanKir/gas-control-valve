#include "Solenoid.h"

void Solenoid_init(void) {
    P5DIR |= BIT4;
    P5OUT &= ~BIT4;
}

void Solenoid_on(void) {
    P5OUT |= BIT4;
}

void Solenoid_off(void) {
    P5OUT &= ~BIT4;
}
