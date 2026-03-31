#include <msp430fr2153.h>
#include <stdbool.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "lcd_ui.h"
#include "RTC.h"
#include "lcd_menu_system.h"
#include "utils.h"

int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init to 16MHz
    init_CLK();

    // LCD startup / init | 4-bit, 2-line operation for now
    LCD_init_4bit();

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


    while(1) 
    {
        if((P3IN & BIT5) != 0)
        {
            menu_action(UP);
        }
        __delay_cycles(1000000);
        if((P3IN & BIT6) != 0)
        {
            menu_action(DOWN);
        }
        __delay_cycles(1000000);
        if((P3IN & BIT7) != 0)
        {
            menu_action(PRESS);
        }
        __delay_cycles(1000000);

    }

    return 0;
}
