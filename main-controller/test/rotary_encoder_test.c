/**************************************
* Author:   Gabe Story
* Date:     03.30.2026
* Class:    EELE 465
* Purpose:  This is the test file for the rotary encoder scrolling
            functionality

************************************************************************/

#include <msp430fr2153.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "LCD_Driver.h"
#include "rotary_encoder.h"

//bool rotary_CW  = false;
//bool rotary_CCW = false;

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
    init_rotary_sw();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    

    while(1) {

        poll_rotary_rotation(cursor_col, &cursor_row);
        poll_rotary_switch();

        if(rotary_CW) {
            cursor_row++;
            rotary_CW = false;
        }

        if(rotary_CCW) {
            cursor_row--;
            rotary_CCW = false;
        }

        if(rotary_switch) {
            LCD_write_string("Hello World! :) ");
            rotary_switch = false;
        }

        LCD_set_cursor(cursor_col, cursor_row);

    }
}
