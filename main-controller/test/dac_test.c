#include <msp430fr2153.h>
#include <stdint.h>
#include <stdbool.h>
#include "eUSCI.h"
#include "utils.h"
#include "dac.h"
#include "gpio.h"

int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // --------- INIT -----------
    init_CLK();
    init_SPI();
    init_dac();

    // Final Init 
    PM5CTL0 &= ~LOCKLPM5;
    __enable_interrupt();

    while(1)
    {
        dac_write(255);        // Check SPI polarity and phase during integration!!
        __delay_cycles(8000000);
        dac_write(128);
        __delay_cycles(8000000);
    }
}