/************************************
* Author: Gabe Story, Griffin Short
* Date: Saturday, 02.21.2026
* Class:   EELE 465
* Purpose: header file to take control of uART protocol driver
*******************************************************************/
#ifndef eUSCI_H
#define eUSCI_H

#include <stdint.h>

// I2C Interrupts
#define RXIFG0 0x16
#define TXIFG0 0x18

// UART Interrupts
#define TXCPTIFG 0x08
#define RXIFG 0x02

typedef enum
{
    I2C_READ,
    I2C_WRITE
} I2C_MODE;

void init_eUSCI_A1_uart();
void init_eUSCI_B0_i2c();
void set_eUSCI_B0_slave_addr(uint8_t);
void set_eUSCI_B0_count(uint8_t);
void init_SPI();

#endif
