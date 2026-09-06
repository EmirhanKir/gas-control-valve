#ifndef THERMOCOUPLE_H_
#define THERMOCOUPLE_H_

#include <msp430.h>

void thermocouple_init(void);
char thermocouple_read(void);

#endif
