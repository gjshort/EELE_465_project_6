/************************************
* Author: Gabe Story, Griffin Short
* Date: Saturday, 02.21.2026
* Class:   EELE 465
* Purpose: c file to take control of uART policy
*******************************************************************/

#include <msp430fr2153.h>
#include "eUSCI.h"

/**
 * Initialize the A1 eUSCI port for UART
 * Using P4.2/3 for Rx/Tx @ 57600 baud
 */
void init_eUSCI_A1_uart() {

	UCA1CTLW0 |= UCSWRST;
	UCA1CTLW0 |= UCSSEL__SMCLK;         // 16 MHz reference clock
    // 57600 baud
	UCA1BRW = 17;
	UCA1MCTLW |= 0xDD51;

    P4SEL1 &= ~BIT2;                    // Port 4.2 for uART Rx
    P4SEL0 |= BIT2;

	P4SEL1 &= ~BIT3;                    // Port 4.3 for uART Tx
	P4SEL0 |= BIT3;

	UCA1CTLW0 &= ~UCSWRST;

}

/**
 * Initialize the B0 eUSCI port for I2C
 * Using P1.2/3 for SDA/SCL
 */
void init_eUSCI_B0_i2c() {                  

    UCB0CTLW0 |= UCSWRST;

    UCB0CTLW0 |= UCSSEL__SMCLK;         // 16 MHz reference clock
    UCB0BRW = 160;                      // Divide by 160 for 100 kHz

    UCB0CTLW0 |= UCMODE_3;              // I2C Mode
    UCB0CTLW0 |= UCMST;                 // Master

    UCB0CTLW1 |= UCASTP_2;              // Auto stop

    P1SEL1 &= ~BIT3;                    // P1.3 for SCL
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT2;                    // P1.2 for SDA
    P1SEL0 |= BIT2;

    UCB0CTLW0 &= ~UCSWRST;              // take eUSCI_B0 out of software reset

    UCB0IE |= UCTXIE0;                  //Enable wait for Tx msg IRQ
    UCB0IE |= UCRXIE0;                  //Enable Rx IRQ

}

/**
 * Sets the slave address for eUSCI B0
 * @param slave_addr - the address of the I2C slave
 */
void set_eUSCI_B0_slave_addr(uint8_t slave_addr)
{
    UCB0CTLW0 |= UCSWRST;
    UCB0I2CSA = slave_addr;             // I2C slave address
    UCB0CTLW0 &= ~UCSWRST;
    UCB0IE |= UCTXIE0;                  //Enable wait for Tx msg IRQ
    UCB0IE |= UCRXIE0;   
}

/**
 * Sets the count of bytes to be read or
 * received before generating an auto-stop
 * @param tbcnt - the number of bytes to be read/written
 */
void set_eUSCI_B0_count(uint8_t tbcnt)
{
    UCB0CTLW0 |= UCSWRST;
    UCB0TBCNT = tbcnt;
    UCB0CTLW0 &= ~UCSWRST;
    UCB0IE |= UCTXIE0;                  //Enable wait for Tx msg IRQ
    UCB0IE |= UCRXIE0;   
}

// Initialize SPI for blasting that LED Stick
void init_SPI() {

    // Set pin for SPI SIMO on UCA0
    P1SEL0 |= BIT7;
    P1SEL1 &= ~BIT7;

    UCA0CTLW0 |= UCSWRST;           // enable software reset

    UCA0CTLW0 &= ~UCCKPL;
    UCA0CTLW0 |= UCCKPH;
    UCA0CTLW0 |= UCMSB;             // configure to 3-pin SPI, master, tx MSB first
    UCA0CTLW0 |= UCMST;
    UCA0CTLW0 |= UCSYNC;

    UCA0CTLW0 |= UCSSEL__SMCLK;     // prescale to 2.667 MHz
    UCA0BRW = 6;

    UCA0CTLW0 &= ~UCSWRST;          // take out of software reset

}