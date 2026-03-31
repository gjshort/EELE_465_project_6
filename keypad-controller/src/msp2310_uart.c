/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the c file for msp430fr2310
            uART communication
    
 **************************************************************/

#include <msp430fr2310.h>
#include <stdint.h>
#include "msp2310_utils.h"
#include "msp2310_uart.h"

/**
* Assign uART pins for msp2310
*/
uART_interface tx_rx_pin = {

    .tx_2310 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT7},
    .rx_2310 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT6}

};

/**
* Init uART for SMCLK, 115200 baud rate
*/
void init_msp2310_uart() {

	UCA0CTLW0 |= UCSWRST;
	UCA0CTLW0 |= UCSSEL__SMCLK;
	UCA0BRW = 8;
	UCA0MCTLW |= 0xD600;
    UCA0CTLW0 &= ~UCSWRST;

}

/**
* General function to set uART pin functionality
*/
void init_msp2310_uart_pins(msp2310_port *p) {

    *(p->port_sel1) &= ~(p->bit_mask);
    *(p->port_sel0) |=  (p->bit_mask);
    
}

/**
* actual function to init uART pins Tx & Rx
*/
void init_msp2310_tx_rx() {

    init_msp2310_uart_pins(&tx_rx_pin.tx_2310);
    init_msp2310_uart_pins(&tx_rx_pin.rx_2310);

}

