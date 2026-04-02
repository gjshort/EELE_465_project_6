/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the test file for heartbeat led interrupt
            for the msp2310

 **************************************************************/

#include <msp430fr2310.h>
#include <stdint.h>
#include "msp2310_utils.h"
#include "msp2310_heartbeat.h"

int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init timer, heartbeat led
    init_msp2310_timer();
    init_heartbeat_led_2310();

    // Enable IE
    __enable_interrupt();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    while(1) {}
}

/**
* This routine will simply wait 1 second to toggle heartbeat led on msp2310
*/
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void){

    heartbeat_toggle();

    TB0CCTL0 &= ~CCIFG;             // Clear CCR0 Flag

}
