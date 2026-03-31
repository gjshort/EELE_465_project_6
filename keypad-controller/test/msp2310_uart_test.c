/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the test file for uART interrupt
            for the msp2310

**************************************************************/

#include <msp430fr2310.h>
#include <stdint.h>
#include "msp2310_utils.h"
#include "msp2310_uart.h"

char string[] = "Hello World! ";
int i, j, position;

 int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init timer, heartbeat led
    init_msp2310_uart();
    init_msp2310_tx_rx();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    while(1) {

        for(position = 0; position < sizeof(string); position++){
	        UCA0TXBUF = string[position];
	        for(i=0; i<100; i=i+1) {}
	    }
	    for(j=0; j<30000; j=j+1) {}

    }
}

