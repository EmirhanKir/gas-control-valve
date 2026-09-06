#ifndef THERMOSTAT_H_
#define THERMOSTAT_H_

#include <msp430.h>
#include <stdint.h>

void Thermostat_init(void);
void Thermostat_update(uint16_t currentTemp);

#endif /* THERMOSTAT_H_ */
