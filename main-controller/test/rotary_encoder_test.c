/**************************************
* Author:   Gabe Story
* Date:     03.30.2026
* Class:    EELE 465
* Purpose:  This is the test file for the rotary encoder scrolling
            functionality

************************************************************************/

#include <msp430fr2153.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "rotary_encoder.h"

uint8_t cursor_col = 0;
uint8_t cursor_row = 0;

int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init stuffs
    init_CLK();
    LCD_init_4bit();
    LCD_clear();
    init_rotary_A_B();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    

    while(1) {

        poll_rotary(cursor_col, &cursor_row);

    }
}
