#ifndef ADC_H_
#define ADC_H_

#include <msp430.h>
#include <stdint.h>

#define POT_CHANNEL           ADCINCH_5
#define THERMISTOR_CHANNEL    ADCINCH_6
#define THERMOCOUPLE_CHANNEL  ADCINCH_3
#define SOLENOID_PIN          BIT4

void ADC_init(void);
uint16_t ADC_readChannel(uint16_t channel);
uint16_t ADC_readPot(void);
unsigned int ADC_readThermistor(void);
unsigned int ADC_readThermocouple(void);
void Solenoid_control(uint8_t state);

#endif
