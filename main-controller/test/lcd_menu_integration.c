#include <msp430fr2153.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "LCD_Driver.h"
#include "lcd_ui.h"
#include "RTC.h"
#include "lcd_menu_system.h"
#include "utils.h"
#include "rotary_encoder.h"
#include "eUSCI.h"
#include "my_float.h"
#include "lmt87lpm.h"
#include "ring_buffer.h"
#include "Potentiometer.h"
#include "led_bar.h"
#include "gpio.h"
#include "led_pattern.h"
#include "LEDstick.h"
#include "dac.h"

// -------- IRQ flags ---------
// Temp sensor ADC
static volatile bool start_temp_adc;
static volatile bool is_temp_adc_done;
static volatile bool start_slide_adc;
static volatile bool is_slide_adc_done;

// I2C
static volatile bool i2c_rx_irq;
static volatile bool i2c_tx_irq;

// UART
static volatile bool uart_txcpt_irq;
static volatile bool uart_rx_irq;

// Timers
static volatile bool change_led_bar_seg;
static volatile bool update_led_bar_pat_state;

// Cactus Change
static volatile bool update_cactus;

// Poll Rotary Encoder
static volatile bool poll_rotary;

// -------- Globals ----------
// ADC
static volatile uint16_t temp_adc_val;
static volatile uint16_t LEDpot;

// I2C
static volatile I2C_MODE rtc_mode;
static volatile bool write_to_rtc;      // Start write transaction
static volatile bool read_from_rtc;     // Start read transaction

// Counters for TB1
static volatile uint8_t half_sec_cnt;
static volatile uint8_t cactus_count;

// Custom character for LCD UI
static volatile bool cactus_dir_up;
static volatile LCD_Custom_Chars lcd_ui_cactus;

int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    // --------- Init globals ---------
    start_temp_adc = false;
    is_temp_adc_done = false;
    start_slide_adc = false;
    is_slide_adc_done = false;
    change_led_bar_seg = false;
    update_led_bar_pat_state = false;
    i2c_rx_irq = false;
    i2c_tx_irq = false;
    uart_txcpt_irq = false;
    uart_rx_irq = false;
    read_from_rtc = false; 
    LEDpot = 0;
    temp_adc_val = 0;
    half_sec_cnt = 0;
    cactus_count = 0;
    cactus_dir_up = false;
    lcd_ui_cactus = Custom_TinyCactus;
    update_cactus = true;
    poll_rotary = false;

    // --------- Init locals ----------
    // MENU SYSTEM
    bool on_main_screen = true;

    // Temp. sensor
    uint16_t lmt87_temp;
    ring_buffer temp_buf = {{0}, 0};    // Ring buffer of temps
    uint8_t temp_avg_window = 5;
    float lmt87_temp_avg = 0;
    char lmt87_temp_str[8] = {0};       // Avg. temp in ASCII

    // Slide pot.
    bool adc_busy = false;

    // I2C
    // Initial time: 10:00:00, Thursday, Feb 19, 2026
    MCP7940N_time rtc_time = {0,0,0x10,0x05,0x20,0x05,0x26};
    uint8_t rtc_reg_idx = 0x00;
    rtc_mode = I2C_WRITE;       // Write initial time to RTC
    write_to_rtc = true;

    // UART
    char keypad_chars[8];
    bool rx_keypad = false;
    uint8_t uart_rx_msg_idx = 0;

    // DAC
    uint8_t lcd_contrast = 50;

    // LEDs
    volatile LED_BAR led_bar = {.red = true, .green = false, 
                                .seg_data = {0}, 
                                .seg_ptr = 0};

    LED_PATTERN led_bar_pattern = { .pattern_num = 0,
                                    .pattern_dir = 0,
                                    .pattern = 0x0AAA };

    // LED Stick
    uint8_t led_stick_g = 140;
    uint8_t led_stick_r = 110;
    uint8_t led_stick_b = 0;

    // Buttons
    bool is_pattern_change_button_low = true;
    bool is_color_change_button_low = true;


    // ------------- INIT MCU -----------------
    // Setup CLK
    init_CLK();

    // Init GPIO
    init_gpio();
    init_led_bar();
    init_led_pattern_timer();

    // Slide Potentiometer ADC
    init_ADCpot();
    // Temperature sensor ADC 
    init_adc_a4();

    // I2C
    init_eUSCI_B0_i2c();
    set_eUSCI_B0_slave_addr(MCP7940N_I2C_ADDR);
    
    // SPI
    init_SPI();

    // UART
    init_eUSCI_A1_uart();
       
    // LCD startup / init | 4-bit, 2-line operation for now
    LCD_init_4bit();
    init_dac();

    // Rotary Encoder
    init_rotary_A_B();
    init_rotary_sw();

    __enable_interrupt();

    // Begin Displays
    clearStick();
    LCD_cursor_off();
    LCD_clear();
    dac_write(lcd_contrast);

    // Initial LCD UI Write
    lcd_ui_write_pattern(led_bar_pattern.pattern_num);
    lcd_ui_write_period(TB0CCR0 / (float)TB0_1_SEC);
    lcd_ui_write_temp(lmt87_temp_avg);
    lcd_ui_write_date_time(&rtc_time);
    lcd_ui_write_window(temp_avg_window);

    // LCD Custom characters
    LCD_load_multiple_chars(Customs, 8);
    LCD_home();

    while(1) 
    {

        // -------------- CACTUS! -----------------
        if(update_cactus && on_main_screen)
        {
            // Cactus moving upwards
            if(cactus_dir_up)
            {
                // Cactus fully upright
                if(lcd_ui_cactus == Custom_Cactus)
                {
                    cactus_dir_up = false;
                    lcd_ui_cactus = Custom_TinyCactus;
                }
                else 
                {
                    lcd_ui_cactus--;
                }
            }
            else
            {
                // Cactus fully downward
                if(lcd_ui_cactus == Custom_Cactus_down)
                {
                    cactus_dir_up = true;
                    lcd_ui_cactus = Custom_TinyCactus_down;
                }     
                else 
                {
                    lcd_ui_cactus++;
                }       
            }
            
            LCD_set_cursor(19, 0);
            LCD_type_char(lcd_ui_cactus);
            update_cactus = false;
        }
        

        // -------------- GPIO --------------------
        // BUTTON - Change LED bar color
        if((P3IN & BIT7) != 0 && is_color_change_button_low == true)
        {
            led_bar_change_color(&led_bar);
            int i;
            for (i = 5000; i > 0; i--) {}
            is_color_change_button_low = false;
        }
        else if((P3IN & BIT7) == 0 && is_color_change_button_low == false)
        {
            is_color_change_button_low = true;
            int i;
            for (i = 5000; i > 0; i--) {}
        }

        // BUTTON - Change Pattern displayed on LED bar
        if((P3IN & BIT6) != 0 && is_pattern_change_button_low == true)
        {
            // Update LED bar data
            led_pat_change_pattern(&led_bar_pattern);
            led_bar_pat_to_anodes(&led_bar_pattern, &led_bar);
            
            // Update LCD screen with pattern data
            if(on_main_screen)
            {
                lcd_ui_write_pattern(led_bar_pattern.pattern_num);
                lcd_ui_write_period(TB0CCR0 / (float)TB0_1_SEC);
            }

            int i;
            for (i = 5000; i > 0; i--) {}
            is_pattern_change_button_low = false;
        }
        else if((P3IN & BIT6) == 0 && is_pattern_change_button_low == false)
        {
            is_pattern_change_button_low = true;
            int i;
            for (i = 5000; i > 0; i--) {}
        }

        // BUTTON - Reset System
        if((P3IN & BIT5) != 0)
        {   
            // Go to pattern 0
            led_bar_pattern.pattern_num = 5;
            led_pat_change_pattern(&led_bar_pattern);

            if(on_main_screen)
            {
                lcd_ui_write_pattern(led_bar_pattern.pattern_num);
            }
            
            clearStick();
            TB0CCTL0 |= CCIE;     // Restart pattern state timer IRQ
        }

        // Activate the next LED bar segment
        if(change_led_bar_seg == true)
        {
            led_bar_clear_anodes();
            led_bar_set_cathode(&led_bar);
            led_bar_write_anodes(&led_bar);
            // Activate next segment on the LED bar
            if(led_bar.seg_ptr <= 1) 
            {
                led_bar.seg_ptr++;
            }
            else 
            {
                led_bar.seg_ptr = 0;
            }
            change_led_bar_seg = false;
        }

        // Change pattern state and update LED bar segment data
        if(update_led_bar_pat_state == true)
        {
            led_pat_change_pattern_state(&led_bar_pattern);
            led_bar_pat_to_anodes(&led_bar_pattern, &led_bar);
            update_led_bar_pat_state = false;
        }

        // --------------- I2C --------------------
        // Start a write
        if(write_to_rtc)
        {
            write_to_rtc = false;
            set_eUSCI_B0_count(RTC_NUM_TIME_REGS + 1);  // Write reg ptr + time registers
            UCB0CTLW0 |= UCTR;                          // Set B0 I2C to Tx mode
            UCB0CTLW0 |= UCTXSTT;                       // Start transmission
        }

        // Transmit register pointer preparing for read
        if(read_from_rtc)
        {
            read_from_rtc = false;
            set_eUSCI_B0_count(1);
            UCB0CTLW0 |= UCTR;
            UCB0CTLW0 |= UCTXSTT;
        }

        // Handle I2C Rx IRQ
        if(i2c_rx_irq)
        {
            i2c_rx_irq = false;
            rtc_read_time_reg(&rtc_time, &rtc_reg_idx);
            if(rtc_reg_idx == RTC_NUM_TIME_REGS - 1)    // Done reading
            {   
                rtc_reg_idx = 0;
                if(on_main_screen)
                {
                    lcd_ui_write_date_time(&rtc_time);      // Update LCD
                }
                menu_update_time_data(&rtc_time);
            }
            else                                        // Still reading
            {
                rtc_reg_idx++;
            }
            
        }

        // Handle I2C Tx IRQ
        if(i2c_tx_irq)
        {
            i2c_tx_irq = false;
            if(rtc_mode == I2C_WRITE)                   // Writing time to RTC
            {
                rtc_write_time_reg(&rtc_time, &rtc_reg_idx);
                if(rtc_reg_idx == RTC_NUM_TIME_REGS)    // Done writing
                {
                    rtc_reg_idx = 0;
                }
                else                                    // Still writing
                {
                    rtc_reg_idx++;
                }
            }
            else if(rtc_mode == I2C_READ)
            {
                // Load register ptr and wait to finish write
                UCB0TXBUF = RTC_SEC_REG;
                while(!(UCB0IFG & UCSTPIFG)) {}
                UCB0IFG &= ~UCSTPIFG;

                // Start read
                set_eUSCI_B0_count(RTC_NUM_TIME_REGS);
                UCB0CTLW0 &= ~UCTR;
                UCB0CTLW0 |= UCTXSTT;
            } 
        }

        // --------------- ADC --------------------
        // Start Temperature ADC conversion
        if(start_temp_adc && !adc_busy)
        {
            adc_busy = true;
            ADCCTL0 &= ~ADCENC;
            ADCCTL0 &= ~ADCON;
            ADCMCTL0 &= ~ADCINCH;
            ADCMCTL0 |= ADCINCH_4;              // Set ADC source to P1.4
            ADCCTL0 |= ADCON;
            ADCCTL0 |= ADCENC | ADCSC;          // Start ADC conversion
        }
        // When ADC is done, take new temp reading and add to
        // ring buffer then average buffer
        if(is_temp_adc_done)
        {
            uint8_t i;
            // Run through LUT and if ADC val is >= to LUT value, pull its
            // associated temperature
            for(i = 0; i < sizeof(lmt87_temp_table[0])/sizeof(uint16_t); i++)
            {
                if(temp_adc_val >= lmt87_temp_table[1][i])
                {
                    lmt87_temp = lmt87_temp_table[0][i];
                    break;
                }
            }
            
            // Add new value to ring buf, re-average the buffer with
            // a specified window and then convert avg. to string
            ring_buf_push(&temp_buf, lmt87_temp);
            temp_avg_window = menu_get_window_size();
            lmt87_temp_avg = ring_buf_average(&temp_buf, temp_avg_window);

            // Clear string buffer then put new avg. temp into it in ASCII
            for(i = 0; i < sizeof(lmt87_temp_str); i++) 
            {
                lmt87_temp_str[i] = '\0';
            }
            ftoa_2(lmt87_temp_avg, lmt87_temp_str);
            
            if(on_main_screen)
            {
                lcd_ui_write_temp(lmt87_temp_avg);          // Update LCD
            }

            is_temp_adc_done = false;
            adc_busy = false;
        }

        // Start slide pot. ADC conversion
        if(start_slide_adc && !adc_busy)
        {
            adc_busy = true;
            ADCCTL0 &= ~ADCENC;
            ADCCTL0 &= ~ADCON;
            ADCMCTL0 &= ~ADCINCH;
            ADCMCTL0 |= ADCINCH_1;              // Channel A1
            ADCCTL0 |= ADCON;
            ADCCTL0 |= ADCENC | ADCSC;          // enable & start conversion
        }
        // When ADC is done, update LED bar period
        // and LED stick light level
        if(is_slide_adc_done)
        {
            // Change LED Stick level and update LCD
            potStick(LEDpot, led_stick_g, led_stick_r, led_stick_b);
            if(on_main_screen)
            {
                lcd_ui_write_period(TB0CCR0 / (float)TB0_1_SEC);
            }
            is_slide_adc_done = false;
            adc_busy = false;
        }        

        // -------------- UART -------------------
        // Rx IRQ
        if(uart_rx_irq && rx_keypad)
        {
            uart_rx_irq = false;
            if(UCA1RXBUF == '#')
            {
                // Null terminate value
                keypad_chars[uart_rx_msg_idx] = '\0';
                uart_rx_msg_idx = 0;
                rx_keypad = false;
                
                uint8_t keypad_data = (uint8_t)atoi(keypad_chars);
                menu_action(KEY_DATA, keypad_data);

                // Update RTC
                menu_get_time(&rtc_time);
                rtc_verify_struct(&rtc_time);
                rtc_mode = I2C_WRITE;
                write_to_rtc = true;

                // Update Contrast
                lcd_contrast = menu_get_contrast();
                dac_write(lcd_contrast);

                // Update LED Stick Colors
                menu_get_rgb(&led_stick_r, &led_stick_g, &led_stick_b);

                // Zero-out Rx Buffer
                uint8_t i;
                for(i = 0; i < sizeof(keypad_chars); i++)
                {
                    keypad_chars[i] = '\0';
                }

                UCA1IE &= ~UCRXIE;
            }
            else 
            {
                // store received char
                keypad_chars[uart_rx_msg_idx] = UCA1RXBUF;
                uart_rx_msg_idx++;
            }

        }


        // --------------------- MENU SYS INTERACTION --------------------
        
        // Poll rotary switch
        if(poll_rotary)
        {
            poll_rotary = false;
            poll_rotary_switch();
        }
        
        // Scroll down
        if(rotary_CW && !rx_keypad && !on_main_screen) {
            rotary_CW = false;
            menu_action(DOWN, 0);
        }

        // Scroll Up
        if(rotary_CCW && !rx_keypad && !on_main_screen) {
            rotary_CCW = false;
            menu_action(UP, 0);
        }

        // Rotary Switch
        if(rotary_switch && !rx_keypad) {
            rotary_switch = false;
            if(!on_main_screen)
            {
                switch(menu_action(PRESS, 0))
                {
                case 1:
                    rx_keypad = true;
                    UCA1IFG &= ~UCRXIFG;
                    UCA1IE |= UCRXIE;
                    break;
                case 2:
                    on_main_screen = true;
                    LCD_clear();
                    lcd_ui_write_pattern(led_bar_pattern.pattern_num);
                    lcd_ui_write_period(TB0CCR0 / (float)TB0_1_SEC);
                    lcd_ui_write_temp(lmt87_temp_avg);
                    lcd_ui_write_date_time(&rtc_time);
                    lcd_ui_write_window(temp_avg_window);
                    break;
                }

                // Update Cursor
                if(menu_get_cursor())
                {
                    if(menu_get_blink())
                    {
                        LCD_cursor_blink();
                    }
                    else 
                    {
                        LCD_cursor_no_blink();
                    }
                }
                else
                {
                    LCD_cursor_off();
                }

            }
            else
            {
                menu_action(ENTER, 0); 
                on_main_screen = false;
            }
            
        }
        
        
        

    }

    return 0;
}

// -----------------------------------------------------------
// ------------------------ ISRs -----------------------------
// -----------------------------------------------------------

// Rotary encoder spin ISR (Pin A rising edge)
#pragma vector = PORT3_VECTOR
__interrupt void ROTARY_SPIN_ISR()
{
    // If B is low, go CW
    if((P3IN & BIT1) == 0)
    {
        rotary_CW = true;
        rotary_CCW = false;
    }
    else 
    {
        rotary_CW = false;
        rotary_CCW = true;
    }
    
    P3IFG &= ~BIT0;
}

// ADC conversion ISR
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR()
{
    if(start_temp_adc)
    {
        start_temp_adc = false;
        is_temp_adc_done = true;
        temp_adc_val = ADCMEM0;
    }
    else if(start_slide_adc)
    {
        start_slide_adc = false;
        is_slide_adc_done = true;
        LEDpot = ADCMEM0;
    }
}

// Change LED pattern state
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0() {
    update_led_bar_pat_state = true;
    TB0CCTL0 &= ~CCIFG;     //Clear IRQ flag
}

// 50 ms TB1 ISR
#pragma vector = TIMER1_B0_VECTOR
__interrupt void ISR_TB1_CCR0(void) 
{
    TB1CCTL0 &= ~CCIFG;

    // Trigger Every 150 ms
    cactus_count++;
    if(cactus_count >= 3)
    {
        cactus_count = 0;
        update_cactus = true;

    }

    // Trigger every 250 ms
    half_sec_cnt++;
    if(half_sec_cnt == 5)
    {
        poll_rotary = true;
    }

    // Trigger every 0.5 seconds
    if(half_sec_cnt >= 10)
    {
        half_sec_cnt = 0;

        P3OUT ^= BIT2;              // Toggle Heartbeat LED

        read_from_rtc = true;       // Read from RTC
        rtc_mode = I2C_READ;

        start_temp_adc = true;      // Start temp. sensor ADC conversion  
    }

    start_slide_adc = true;         // Start slide pot ADC conversion  
}

// Change segment on LED Bar
#pragma vector = TIMER2_B0_VECTOR
__interrupt void ISR_TB2_CCR0() {
    change_led_bar_seg = true;
    TB2CCTL0 &= ~CCIFG;     //Clear IRQ flag
}

// I2C ISR
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void) 
{
    switch(UCB0IV) 
    {
    case RXIFG0:            // Received byte
        i2c_rx_irq = true;
        break;
    case TXIFG0:            // Need to load byte
        i2c_tx_irq = true;
        break;
    default:
        break;
    }
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
