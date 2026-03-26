#include <msp430fr2153.h>
#include <stdint.h>
#include <stdbool.h>
#include "eUSCI.h"
#include "rtc.h"
#include "utils.h"
#include "lmt87lpm.h"
#include "ring_buffer.h"
#include "my_float.h"
#include "Potentiometer.h"
#include "led_bar.h"
#include "gpio.h"
#include "utils.h"
#include "led_pattern.h"
#include "LEDstick.h"
#include "LCD_Driver.h"
#include "lcd_ui.h"
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
    // Disable low power mode
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

    // --------- Init locals ----------
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
    bool uart_start_time_tx = false;
    bool uart_start_temp_tx = false;
    bool uart_tx_busy = false;
    uint8_t uart_tx_msg_idx = 0x00;
    uint8_t uart_rx_msg_idx = 0x00;
    char uart_tx_msg_buf[24] = {0};
    char uart_rx_msg_buf[24] = {0};

    // DAC
    uint8_t lcd_contrast = 0;

    // LEDs
    volatile LED_BAR led_bar = {.red = true, .green = false, 
                                .seg_data = {0}, 
                                .seg_ptr = 0};

    LED_PATTERN led_bar_pattern = { .pattern_num = 0,
                                    .pattern_dir = 0,
                                    .pattern = 0x0AAA };

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
    UCA1IFG &= ~UCRXIFG;
    UCA1IE |= UCRXIE;       // Enable RX IRQ

    // LCD Screen
    LCD_init_4bit();
    init_dac();

    // Enable Global IRQs
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
    LCD_load_multiple_chars(Customs, 5);
    LCD_home();
    

    while(true)
    {
        // -------------- CACTUS! -----------------
        if(update_cactus)
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
            lcd_ui_write_pattern(led_bar_pattern.pattern_num);
            lcd_ui_write_period(TB0CCR0 / (float)TB0_1_SEC);

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
            lcd_ui_write_pattern(led_bar_pattern.pattern_num);
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
                lcd_ui_write_date_time(&rtc_time);      // Update LCD
                uart_start_time_tx = true;              // Start UART Tx of time
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
            lmt87_temp_avg = ring_buf_average(&temp_buf, temp_avg_window);

            // Clear string buffer then put new avg. temp into it in ASCII
            for(i = 0; i < sizeof(lmt87_temp_str); i++) 
            {
                lmt87_temp_str[i] = '\0';
            }
            ftoa_2(lmt87_temp_avg, lmt87_temp_str);
            lcd_ui_write_temp(lmt87_temp_avg);          // Update LCD
            uart_start_temp_tx = true;

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
            potStick(LEDpot);
            lcd_ui_write_period(TB0CCR0 / (float)TB0_1_SEC);
            is_slide_adc_done = false;
            adc_busy = false;
        }        

        // -------------- UART -------------------
        // Start Tx of time after an RTC read completed
        if(uart_start_time_tx && !uart_tx_busy)
        {
            uart_start_time_tx = false;   
            uart_tx_busy = true;
            TB1CCTL0 &= ~CCIE;          // Don't let TB1 trigger during Tx

            // Enable UART IRQ
            UCA1IFG &= ~UCTXCPTIFG;     
            UCA1IE |= UCTXCPTIE;

            // Pack time buffer and start transmit
            pack_time_buffer(&rtc_time, uart_tx_msg_buf);
            uart_tx_msg_data(uart_tx_msg_buf, uart_tx_msg_idx);
            uart_tx_msg_idx++;
        }

        // Start Tx of temp. afer an ADC read completed
        if(uart_start_temp_tx && !uart_tx_busy)
        {
            uart_start_temp_tx = false;
            uart_tx_busy = true;
            TB1CCTL0 &= ~CCIE;          // Don't let TB1 trigger during Tx

            // Enable UART IRQ
            UCA1IFG &= ~UCTXCPTIFG;     
            UCA1IE |= UCTXCPTIE;

            // Pack data buffer with temp. and start transmit
            pack_temp_buffer(lmt87_temp_str, uart_tx_msg_buf);
            uart_tx_msg_data(uart_tx_msg_buf, uart_tx_msg_idx);
            uart_tx_msg_idx++;
        }

        // TXCPT IRQ
        if(uart_txcpt_irq)
        {
            uart_txcpt_irq = false;
            if(uart_tx_msg_buf[uart_tx_msg_idx] == '\0')
            {   // Reset index, disable UART IRQ, enable TB1 IRQ
                uart_tx_msg_idx = 0;
                uart_tx_busy = false;
                UCA1IE &= ~UCTXCPTIE;
                TB1CCTL0 &= ~CCIFG;     
                TB1CCTL0 |= CCIE;
            }
            else 
            {   // Load next character. After, increment index
                uart_tx_msg_data(uart_tx_msg_buf, uart_tx_msg_idx);
                uart_tx_msg_idx++;
            }
        }

        // Rx IRQ
        if(uart_rx_irq)
        {
            uart_rx_irq = false;
            if(UCA1RXBUF == '\n')
            {   // Reset index, parse message
                uart_rx_msg_idx = 0;
                char msg_id = parse_uart_msg(uart_rx_msg_buf);
                switch(msg_id)
                {
                case ID_TIME:
                    parse_uart_time_msg(uart_rx_msg_buf, &rtc_time);
                    rtc_mode = I2C_WRITE;
                    write_to_rtc = true;
                    break;
                case ID_WINDOW:
                    parse_uart_window_msg(uart_rx_msg_buf, &temp_avg_window);
                    lcd_ui_write_window(temp_avg_window);
                    break;
                case ID_CONTRAST:
                    parse_uart_contrast_msg(uart_rx_msg_buf, &lcd_contrast);
                    dac_write(lcd_contrast);
                    break;
                case ID_ERR:
                    break;
                default:
                    break;
                }
                
                // Zero-out Rx Buffer
                uint8_t i;
                for(i = 0; i < sizeof(uart_rx_msg_buf); i++)
                {
                    uart_rx_msg_buf[i] = '\0';
                }
            }
            else 
            {   // Store received byte into buffer
                uart_rx_msg_buf[uart_rx_msg_idx] = UCA1RXBUF;
                uart_rx_msg_idx++;
            }
        }

    }
    
}

// -----------------------------------------------------------
// ------------------------ ISRs -----------------------------
// -----------------------------------------------------------

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

    cactus_count++;
    if(cactus_count >= 3)
    {
        cactus_count = 0;
        update_cactus = true;
    }

    // Trigger every 0.5 seconds
    half_sec_cnt++;
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
    case TXCPTIFG:              // Done transmitting byte
        UCA1IFG &= ~UCTXCPTIFG; // Clear IRQ flag
        uart_txcpt_irq = true;
        break;
    case RXIFG:                 // Receieved byte
        UCA1IFG &= ~UCRXIFG;    // Clear IRQ flag
        uart_rx_irq = true;
        break;
    default:
        break;
    }

}
