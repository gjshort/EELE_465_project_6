// Header for the MCP4801 8-bit DAC with SPI

#ifndef DAC_H
#define DAC_H

#include <stdint.h>
#include <stdbool.h>

void init_dac();
void dac_write(uint8_t data);

#endif
