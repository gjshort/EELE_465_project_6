/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the test file for keypad inputs running
            with heartbeat led and keypress led

 **************************************************************/

 #include <msp430fr2310.h>
 #include <stdint.h>
 #include "msp2310_utils.h"
 #include "msp2310_heartbeat.h"
 #include "keypad.h"

char x;             // test variable
char y = '0';

 int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init
    keypad_column_init();
    keypad_led_init();
    init_msp2310_timer();
    init_heartbeat_led_2310();

    // Enable IE
    __enable_interrupt();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    while(1) {

        x = poll_keypad();

        if(x != 0) {
            y = x;
        }

    }
}

// --------------------------------- TIMER IE -----------------------------------------------
/**
* This routine will simply wait 1 second to toggle heartbeat led on msp2310
*/
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void){

    heartbeat_toggle();

    TB0CCTL0 &= ~CCIFG;             // Clear CCR0 Flag

}
