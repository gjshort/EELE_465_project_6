/****************************************
*  Author:  Gabe Story
*    Date:  03.12.2026
*   Class:  EELE 465
* Purpose:  This is the header file for all LCD_driver
            functionality.
    
 **************************************************************/

#include    <msp430fr2153.h>
#include    <stdint.h>

#pragma once

// Struct for pin dir/out and bitmask
typedef struct {

    volatile    uint8_t *port_dir;      // pointer to PxDIR
    volatile    uint8_t *port_out;      // pointer to PxOUT
                uint8_t  bit_mask;      // BITx select

} LCD_pin;

// Struct for specific interfacing w/ NH LCD
typedef struct {

    LCD_pin RS;
    LCD_pin  E;
    LCD_pin D4;
    LCD_pin D5;
    LCD_pin D6;
    LCD_pin D7;

} LCD_interface;

// enum struct for storing custom char names to CGRAM address 0 - 7
typedef enum {

    Custom_Cactus           = 0,
    Custom_TinyCactus       = 1,
    Custom_Dash             = 2,
    Custom_TinyCactus_down  = 3,
    Custom_Cactus_down      = 4,
    Custom_Brace            = 5,
    Custom_Queen            = 6,
    Custom_Indicator        = 7

} LCD_Custom_Chars;

// Let main know we have a 2-D array in another .c file.
extern const char Customs[8][8];

// Implicit funtion declarations
void LCD_set_pin_high(LCD_pin *p);
void LCD_set_pin_low(LCD_pin *p);
void LCD_pulse_enable();
void LCD_write_bus(uint8_t nibble);
void LCD_write_init_nibble(uint8_t nibble);
void LCD_write_byte(uint8_t data, uint8_t rs);
void LCD_init_4bit();
static void LCD_cmd(uint8_t cmd);
void LCD_clear();
void LCD_home();
void LCD_cursor_blink();
void LCD_cursor_no_blink();
void LCD_cursor_off();
void LCD_set_cursor(uint8_t col, uint8_t row);
void LCD_type_char(char c);
void LCD_write_string(const char *s);
void LCD_write_string_here(uint8_t col, uint8_t row, const char *s);
extern void LCD_custom_char(char address, const char *custom_map);
extern void LCD_load_multiple_chars(const char Customs_CGRAM[][8], uint8_t addr_nmbr);
