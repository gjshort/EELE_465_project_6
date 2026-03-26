#include <msp430fr2153.h>
#include <stdint.h>
#include <stdbool.h>
#include "dac.h"

/*
* DAC data packet structure for the MCP4801 (8-bit)
* (x = don't care)
*   |----------------- BYTE 1 ---------------||--------- BYTE 2 ---------|
*   [Write, x, Gain, Shutdown, D7, D6, D5, D4][D3, D2, D1, D0, x, x, x, x]
*
* Send Byte 1 (MSB) first
*/

// Write bit: 0 = Write to DAC
// In upper byte of frame sent to DAC
#define DAC_WRITE_BIT 0x80

// Gain bit: 1: Vo = (Vref   * D/4096)
//           0: Vo = (2 * Vref * D/4096)
// in upper byte of frame sent to DAC
#define DAC_GAIN_BIT 0x20

// Shutdown bit: 1 = Active mode
// In upper byte of frame sent to DAC
#define DAC_SHDN_BIT 0x10

static const uint8_t DAC_CONFIG_BITS = DAC_GAIN_BIT | DAC_SHDN_BIT & ~(DAC_WRITE_BIT);

#define DAC_CS_PORT P3OUT
#define DAC_CS_PIN BIT3

/**
 * Inits the chip select pin on the MSP for the DAC
 * CS is active low on MCP4801
 */
static void init_dac_cs_pin()
{
    // Using P3.3
    P3SEL0 &= ~BIT3;    // Set to Digital IO
    P3SEL1 &= ~BIT3;
    P3DIR |= BIT3;      // Set as output
    P3OUT |= BIT3;      // Set output
}

/**
 * Inits the pin used for the DAC's SPI clock
 */
static void init_dac_sck_pin()
{
    P1SEL0 |=  BIT5;    // Set to UCA0CLK
    P1SEL1 &= ~BIT5;
    P1DIR |= BIT5;      // Set as output
}

/**
 * Calls all DAC init functions.
 */
void init_dac()
{
    init_dac_cs_pin();
    init_dac_sck_pin();
}

/**
 * Writes an 8 bit value to the DAC
 * @param data - value corresponding to desired voltage
 *
 * Depending on gain bit, 'data' will map to a different
 * range of values:
 *
 * Gain = 1x: Data[0 : 255] --> Vout[0V : 2.048V]
 * Gain = 2x: Data[0 : 255] --> Vout[0V : 4.096V]
 */
void dac_write(uint8_t data)
{
    uint8_t byte_1 = DAC_CONFIG_BITS | ((data & 0xF0) >> 4);
    uint8_t byte_2 = (data & 0x0F) << 4;

    // Send CS low, write data, send CS high
    DAC_CS_PORT &= ~(DAC_CS_PIN);

    while(!(UCA0IFG & UCTXIFG) || (UCA0STATW & UCBUSY));    // Wait for Tx buf to clear
    UCA0TXBUF = byte_1;
    while(!(UCA0IFG & UCTXIFG) || (UCA0STATW & UCBUSY));    // Wait for Tx buf to clear
    UCA0TXBUF = byte_2;
    while(!(UCA0IFG & UCTXIFG) || (UCA0STATW & UCBUSY));    // Wait for Tx buf to clear

    DAC_CS_PORT |= DAC_CS_PIN;
}
