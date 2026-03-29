/****************************************
*  Author:  Gabe Story
*    Date:  03.12.2026
*   Class:  EELE 465
* Purpose:  This is the demo file that explicitly shows
            requirements 1-7.

            1. Display all characters in the ROM table
            between 00100000 and 01111111.

            2. Print to all segment of the 20x4 display.

            3. Turn cursor on and off.

            4. Turn cursor blinking on and off.

            5. set cursor position w/o writing characters

            6. Clear the LCD display

            7. Write a custom character
                7.1 - integrate into main program
    
 **************************************************************/

#include <msp430fr2153.h>
#include <stdbool.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "utils.h"


int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init to 16MHz
    init_CLK();

    // LCD startup / init | 4-bit, 2-line operation
     LCD_init_4bit();
         LCD_clear();

    // Load custom chars into CGRAM
    LCD_load_multiple_chars(Customs, 5);
    LCD_home();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    // Step through in the debugger to see all functionality

// -------- 1. Display all characters in the ROM table between 00100000 and 01111111. 
    LCD_cursor_off();

    // Alphabet
    LCD_write_string("abcdefghijklmnopqrst");
       LCD_write_string_here(0, 1, "uvwxyz");

    LCD_write_string_here(0, 2, "ABCDEFGHIJKLMNOPQRST");
                  LCD_write_string_here(0, 3, "UVWXYZ");

    LCD_clear();

    // Numbers, symbols
    LCD_write_string("0123456789");
    LCD_write_string_here(0, 1, "!?.,:;<>*-+()'\"@#$^%");
    LCD_write_string_here(0, 2, "`[]{}|&_=");             
    LCD_type_char(0x5C);                                        // easiest way to type yen symbol
    LCD_type_char(0x7E);                                        // easiest way to type ->
    LCD_type_char(0x7F);                                        // easiest way to type <-

// -------- 2. Print to all segments of 20x4 display
    LCD_clear();
    LCD_write_string("....................");
    LCD_write_string("////////////////////");
    LCD_write_string("....................");
    LCD_write_string("////////////////////");

// -------- 3 & 4. Turn cursor on and off, blink on and off
    LCD_clear();
    LCD_cursor_no_blink();
    LCD_cursor_blink();
    LCD_cursor_off();

// -------- 5. Set cursor position anywhere w/o writing characters
//    LCD_cursor_no_blink();
//   
//    uint8_t col = 0;
//    for(col = 0; col <= 19; col++) {
//
//         LCD_set_cursor(col, 0);
//        __delay_cycles(4000000);
//         LCD_set_cursor(col, 1);
//        __delay_cycles(4000000);
//         LCD_set_cursor(col, 2);
//        __delay_cycles(4000000);
//         LCD_set_cursor(col, 3);
//        __delay_cycles(4000000);
//
//    }

// -------- 6. Clear display, now stepping through the reqs I already do this, but here is an explicit test.
    LCD_cursor_off();
         LCD_clear();

    LCD_write_string("|-|-|-|-|-|-|-|-|-|-");
    LCD_write_string("|-|-|-|-|-|-|-|-|-|-");
    LCD_write_string("|-|-|-|-|-|-|-|-|-|-");
    LCD_write_string("|-|-|-|-|-|-|-|-|-|-");

    LCD_clear();

// -------- 7. Custom character. Write custom character loaded into CGRAM address 0 in the top left corner of LCD
    while(1) {

                    LCD_set_cursor(19, 0);
             LCD_type_char(Custom_Cactus);
                  __delay_cycles(2400000);

                    LCD_set_cursor(19, 0);
             LCD_type_char(Custom_TinyCactus);
                  __delay_cycles(2400000);

                    LCD_set_cursor(19, 0);
               LCD_type_char(Custom_Dash);
                  __delay_cycles(2400000);

                    LCD_set_cursor(19, 0);
             LCD_type_char(Custom_TinyCactus_down);
                  __delay_cycles(2400000);

                    LCD_set_cursor(19, 0);
        LCD_type_char(Custom_Cactus_down);
                  __delay_cycles(2400000);
                    LCD_set_cursor(19, 0);

                    LCD_set_cursor(19, 0);
             LCD_type_char(Custom_TinyCactus_down);
                  __delay_cycles(2400000);

                    LCD_set_cursor(19, 0);
               LCD_type_char(Custom_Dash);
                  __delay_cycles(2400000);

                    LCD_set_cursor(19, 0);
             LCD_type_char(Custom_TinyCactus);
                  __delay_cycles(2400000);

    }

}
