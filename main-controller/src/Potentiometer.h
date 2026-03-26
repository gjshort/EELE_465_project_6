#pragma once

typedef struct ADCpot {

    unsigned int    ADCpot_value;

} ADCpot;

void init_ADCpot();
void ADCpot_period(uint16_t LEDpot);
