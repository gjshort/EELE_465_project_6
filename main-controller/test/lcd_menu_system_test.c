#include <msp430fr2153.h>
#include <stdbool.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "lcd_ui.h"
#include "RTC.h"
#include "lcd_menu_system.h"
#include "utils.h"
#include "rotary_encoder.h"

int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init to 16MHz
    init_CLK();

    // LCD startup / init | 4-bit, 2-line operation for now
    LCD_init_4bit();

    // Rotary Encoder
    init_rotary_A_B();
    init_rotary_sw();

    // Test time: 10:00:00, Thursday, Feb 19, 2026
    MCP7940N_time rtc_time = {0,0,0x10,0x05,0x20,0x05,0x26};

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    // Main Screen Init
    LCD_clear();
    LCD_cursor_off();
    /*
    lcd_ui_write_pattern(0);
    lcd_ui_write_period(0.56);
    lcd_ui_write_temp(19.0);
    lcd_ui_write_date_time(&rtc_time);
    */

    LCD_home();

    menu_action(NONE);

    while(1) 
    {

        poll_rotary_rotation();
        poll_rotary_switch();

        if(rotary_CW) {
            rotary_CW = false;
            menu_action(DOWN);
        }

        if(rotary_CCW) {
            rotary_CCW = false;
            menu_action(UP);
        }

        if(rotary_switch) {
            rotary_switch = false;
            menu_action(PRESS);
            __delay_cycles(5000000);
        }
        
        

    }

    return 0;
}
