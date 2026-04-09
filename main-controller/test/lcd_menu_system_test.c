#include <msp430fr2153.h>
#include <stdbool.h>
#include <stdint.h>
#include "LCD_Driver.h"
#include "lcd_ui.h"
#include "RTC.h"
#include "lcd_menu_system.h"
#include "utils.h"
#include "rotary_encoder.h"
#include "eUSCI.h"
#include "my_float.h"
#include "stdlib.h"

// UART
static volatile bool uart_rx_irq;

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

    // UART
    init_eUSCI_A1_uart();
    UCA1IE |= UCRXIE;
    char keypad_chars[8];
    bool rx_keypad = false;
    uint8_t uart_rx_msg_idx = 0;

    // Test time: 10:00:00, Thursday, Feb 19, 2026
    MCP7940N_time rtc_time = {0,0,0x10,0x05,0x20,0x05,0x26};

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    __enable_interrupt();

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

    menu_action(NONE, 0);

    while(1) 
    {

        poll_rotary_rotation();
        poll_rotary_switch();

        if(rotary_CW && !rx_keypad) {
            rotary_CW = false;
            menu_action(DOWN, 0);
        }

        if(rotary_CCW && !rx_keypad) {
            rotary_CCW = false;
            menu_action(UP, 0);
        }

        if(rotary_switch && !rx_keypad) {
            rotary_switch = false;
            if(menu_action(PRESS, 0))
            {
                rx_keypad = true;
            }
            __delay_cycles(5000000);
        }
        
        if(uart_rx_irq && rx_keypad)
        {
            uart_rx_irq = false;
            if(UCA1RXBUF == '#')
            {
                keypad_chars[uart_rx_msg_idx] = '\0';
                uart_rx_msg_idx = 0;
                rx_keypad = false;
                
                uint8_t keypad_data = (uint8_t)atoi(keypad_chars);
                menu_action(KEY_DATA, keypad_data);

                // Zero-out Rx Buffer
                uint8_t i;
                for(i = 0; i < sizeof(keypad_chars); i++)
                {
                    keypad_chars[i] = '\0';
                }
            }
            else 
            {
                keypad_chars[uart_rx_msg_idx] = UCA1RXBUF;
                uart_rx_msg_idx++;
            }

        }
        

    }

    return 0;
}


// UART ISR
#pragma vector = EUSCI_A1_VECTOR
__interrupt void EUSCI_A1_UART_ISR(void) 
{
    switch(UCA1IV)
    {
    case RXIFG:                 // Receieved byte
        UCA1IFG &= ~UCRXIFG;    // Clear IRQ flag
        uart_rx_irq = true;
        break;
    default:
        break;
    }

}
