/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the test file for keypad inputs   
 **************************************************************/

 #include <msp430fr2310.h>
 #include <stdint.h>
 #include "msp2310_utils.h"
 #include "keypad.h"

char x;             // test variable
char y = '0';

 int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Keypad init columns to inputs, pulled down low
    keypad_column_init();
    keypad_led_init();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    while(1) {

        x = poll_keypad();

        if(x != 0) {
            y = x;
        }

    }
}
