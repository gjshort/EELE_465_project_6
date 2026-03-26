/****************************************
*  Author:  Gabe Story
*    Date:  03.12.2026
*   Class:  EELE 465
* Purpose:  This is the c file for all LCD_driver
            functions
    
 **************************************************************/

 #include <msp430fr2153.h>
 #include <stdint.h>
 #include "LCD_Driver.h"

// DDRAM addresses mapping for 4 rows (0, 1, 2, 3) used in LCD_set_cursor()
static const uint8_t LCD_row[4] = {0x00, 0x40, 0x14, 0x54};

// Mapping LCD interface to MSP, 4-bit parallel mode. NO R/W pin, tied to gnd.
LCD_interface NH_LCD = {

    .RS = {&P2DIR, &P2OUT, BIT0},
     .E = {&P2DIR, &P2OUT, BIT1},
    .D4 = {&P2DIR, &P2OUT, BIT2},
    .D5 = {&P2DIR, &P2OUT, BIT3},
    .D6 = {&P4DIR, &P4OUT, BIT0},
    .D7 = {&P4DIR, &P4OUT, BIT1}

};

// Set pins high. Like just set the pins high; all the pins need to be high
void LCD_set_pin_high(LCD_pin *p) {

    *(p->port_dir) |= (p->bit_mask);
    *(p->port_out) |= (p->bit_mask);

}

// Now wait. Some pins might need to be low; maybe all pins need to be low
void LCD_set_pin_low(LCD_pin *p) {

    *(p->port_dir) |=  (p->bit_mask);
    *(p->port_out) &= ~(p->bit_mask);

}

// LCD enable latch pulse function
void LCD_pulse_enable() {

    __delay_cycles(80);                 // data setup time ~5us

    LCD_set_pin_high(&NH_LCD.E);
            __delay_cycles(200);        // Set E high
     LCD_set_pin_low(&NH_LCD.E);
            __delay_cycles(200);        // Trigger E low

}

// LCD write bus, this handles arbitrary bit checking for data / commands 
void LCD_write_bus(uint8_t nibble) {

    /*** WE want to check each bit in our nibble,
    *    wich can be two things, 1 or 0. We use
    *    bit masking to check D7-D4 and set accordingly ***/

    if(nibble & 0x08) {
        LCD_set_pin_high(&NH_LCD.D7);           // if (0bXyyy & 0b1000) = 1, D7 = 1
    } else {                                    
         LCD_set_pin_low(&NH_LCD.D7);           // if (0bXyyy & 0b1000) = 0, D7 = 0
    }

    if(nibble & 0x04) {
        LCD_set_pin_high(&NH_LCD.D6);           // if (0byXyy & 0b0100) = 1, D6 = 1
    } else {                                    
         LCD_set_pin_low(&NH_LCD.D6);           // if (0byXyy & 0b0100) = 0, D6 = 0
    }

    if(nibble & 0x02) {
        LCD_set_pin_high(&NH_LCD.D5);           // if (0byyXy & 0b0010) = 1, D5 = 1
    } else {
         LCD_set_pin_low(&NH_LCD.D5);           // if (0byyXy & 0b0010) = 0, D5 = 0
    }

    if(nibble & 0x01) {
        LCD_set_pin_high(&NH_LCD.D4);           // if (0byyyX & 0b0001) = 1, D4 = 1
    } else {
         LCD_set_pin_low(&NH_LCD.D4);           // if (0byyyX & 0b0001) = 0, D4 = 0
    }

}

// Actual init nibble write to LCD
void LCD_write_init_nibble(uint8_t nibble) {

    LCD_write_bus(nibble);
       LCD_pulse_enable();

}

// Normal / arbitrary 4-bit write to use after init
void LCD_write_byte(uint8_t data, uint8_t rs) {

    // If rs = 1 for data set RS to 1, else set RS = 0 for command
    if(rs) {
        LCD_set_pin_high(&NH_LCD.RS);
    } else {
         LCD_set_pin_low(&NH_LCD.RS);
    }

    // Send upper nibble 
    LCD_write_bus(data >> 4);       // 0bXXXXyyyy --> 0b0000XXXX
          LCD_pulse_enable();       // latch that sucker
         __delay_cycles(400);       // Delay padding

    // Mask and send lower nibble
    LCD_write_bus(data & 0x0F);     // 0bXXXXyyyy & 0b00001111 = 0b0000yyyy
            LCD_pulse_enable();     // latch that sucker

    __delay_cycles(600);            // ~37us

    /***  This makes generic writing to the LCD really simple
          this function takes input arguments 
          (i.e LCD_write_byte('A', 1) or LCD_write_byte(0x01, 0)) 
          and will parse it into a character or command ***/

}

// ACTUAL INIT TO 4 BIT MODE, if you don't include this in your file you're cooked
void LCD_init_4bit() {

     LCD_set_pin_low(&NH_LCD.E);        // Enable pin start low
    LCD_set_pin_low(&NH_LCD.RS);        // RS == 0 || Send commands twin

    __delay_cycles(640000);             // > 40ms wait on startup let LCD wakey wakey

    /*** Okay I learned that the MCU is running way faster
         than the LCD like the ST7066U is slow on startup
         so we gotta like brute force this thing to get into
         8-bit mode until it knows what its doing before we 
         go into 4-bit mode or else LCD init comes across as 
         garbled junk. ***/

    // Force 8-bit mode
    LCD_write_init_nibble(0x3);     // so yeah remember when we only wired D7-D4?
                                    // yeah kinda chopped but we can still send 8-bit mode command
                                    // by sending just the upper nibble since thats all we need (0x3 of 0x30)
           __delay_cycles(800);     // ~37us

    LCD_write_init_nibble(0x3);
           __delay_cycles(800);

    LCD_write_init_nibble(0x3);     // yo we gotta be in 8-bit mode by now, like we all stable and stuff now right?
           __delay_cycles(800);

    // Now enter 4-bit mode
    LCD_write_init_nibble(0x2);     // 0x2 = 0b00100000
           __delay_cycles(800);

    /*** Startup Commands ***/

    // 4-bit , 2 - line , 5x8 font
    LCD_write_byte(0x28, 0);        // 0x28 = 0b00101000
                                    // DL = 0 (4-bit), N = 1 (2-line), F = 0 (5x8 font)

    // Display ON
    LCD_write_byte(0x0F, 0);        // 0x0C = 0b00001111
                                    // D = 1 (display ON), C = 1 (cursor ON), B = 1 (blink ON)

    // Clear Display
    LCD_write_byte(0x01, 0);        
      __delay_cycles(25000);        // ~1.5ms, this command is a big one so gotta give it time to execute

    // Entry Mode
    LCD_write_byte(0x06,0);        
}

// --------------------------------------------------------------- HELPER FUNCTIONALITY ------------------------------------------------------------ //

/*** So like, we got functions that let's you do commands
     and write single characters. Let's use those to make
     some useful functions that help automate the overall process ***/

// CMD function, for more direct way of writing commands to to display
static void LCD_cmd(uint8_t cmd) {
    LCD_write_byte(cmd, 0);
}

// Clear display entirely, this also returns the cursor back to top left.
void LCD_clear() {

            LCD_cmd(0x01);
    __delay_cycles(25000);

}

// Return cursor to the top left of the display
void LCD_home() {

            LCD_cmd(0x02);
    __delay_cycles(25000);

}

// Turn cursor blink on, (cursor ON, blink ON)
void LCD_cursor_blink() {

    LCD_cmd(0x0F);

}

// Turn cursor blink off, (cursor ON, blink OFF)
void LCD_cursor_no_blink() {

    LCD_cmd(0x0E);

}

// Turn cursor off
void LCD_cursor_off() {

    LCD_cmd(0x0C);

}

// This function lets you set the cursor anywhere on the display for a 20x4 address
void LCD_set_cursor(uint8_t col, uint8_t row) {

    if(row > 3) {
        row = 3;        // safety so we don't brick, we ain't got no more than 4 rows!
    }

    if(col > 19) {
        col = 19;       // we ain't got no more than 20 coloumns!
    }

    uint8_t addr = LCD_row[row] + col;
                 LCD_cmd(0x80 | addr);      // 0x80 command sets DDRAM address pointer

}

// Write a single character without having to address RS bit
void LCD_type_char(char c) {

    LCD_write_byte((uint8_t)c, 1);      // (uint8_t)c converts char ---> unsigned 8-bit number

}

/*** Okay, I am about to get FREAKY up in here,
     I am gonna make a function so we can write 
     strings. ***/

void LCD_write_string(const char *s) {

    while(*s) {
        LCD_type_char(*s);      // Since we do not want to over-write other info
                      s++;      // on the screen. Partner brought up NULL-terminated strings.
                                // so that is essentially what this will do! It goes until *s
                                // finds a NULL terminator and then stops writing. 
    }

}

// EXTRA freaky string function, set cursor perhaps???
void LCD_write_string_here(uint8_t col, uint8_t row, const char *s) {

    LCD_set_cursor(col, row);
         LCD_write_string(s);

}

// --------------------------------------------------------------- CUSTOM CHAR FUNCTIONALITY ------------------------------------------------------------ //

/*** Custom char for CGRAM, the idea is in each
     cursor position there is a 5x8 square that
     makes up characters. So we define our own
     character by writing explicitly what we want
     for each row. 1 indicates turning a pixel on,
     0 for pixels to be off. ***/

/*** We can have up to 8 custom characters, to help
     load all custom characters at once we can pass a 2-D
     array into a funciton that will load each custom character
     sequentially in each address. ***/

const char Customs[8][8] = {

    // Custom #0: Cactus, see bit image below
    {
        0x04,       // 0b00100
        0x14,       // 0b10100
        0x15,       // 0b10101
        0x1D,       // 0b11101
        0x07,       // 0b00111
        0x04,       // 0b00100
        0x04,       // 0b00100
        0x04        // 0b00100
    },

    // Custom #1: Tiny cactus
    {
        0x00,       // 0b00000
        0x00,       // 0b00000
        0x14,       // 0b10100
        0x15,       // 0b10101
        0x1D,       // 0b11101
        0x07,       // 0b00111
        0x04,       // 0b00100
        0x00        // 0b00000
    },

    // Custom #2: Dash, see bit image below
    {
        0x00,       // 0b00000
        0x00,       // 0b00000
        0x00,       // 0b00000
        0xFF,       // 0b11111
        0x00,       // 0b00000
        0x00,       // 0b00000
        0x00,       // 0b00000
        0x00        // 0b00000
    },

    // Custom #3: Tiny upside down cactus
    {
        0x00,       // 0b00000
        0x04,       // 0b00100
        0x07,       // 0b00111
        0x1D,       // 0b11101
        0x15,       // 0b10101
        0x14,       // 0b10100
        0x00,       // 0b00000
        0x00        // 0b00000
    },

    // Custom #4: Upside down cactus
    {

        0x04,       // 0b00100
        0x04,       // 0b00100
        0x04,       // 0b00100
        0x07,       // 0b00111
        0x01D,      // 0b11101
        0x015,      // 0b10101
        0x014,      // 0b10100
        0x04        // 0b00100

    },

    // Custom #4: Dash, see bit image below
    {
        0x00,       // 0b00000
        0x00,       // 0b00000
        0x00,       // 0b00000
        0xFF,       // 0b11111
        0x00,       // 0b00000
        0x00,       // 0b00000
        0x00,       // 0b00000
        0x00,       // 0b00000
    }

    
    // Here you would make more... up to 8 and update enum struct in header file accordingly.
    // ...

};

// Load custom char into CGRAM, write to CGRAM address
void LCD_custom_char(char address, const char *custom_map) {

    LCD_cmd(0x40 | (address << 3));         // go to CGRAM memory, address 0

    uint8_t i = 0;                          // write explicit each byte to 5x7 address
    for(i = 0; i < 8; i++) {
        LCD_type_char(custom_map[i]);
    }

}

// Helper function to laod multiple custom chars at once.
void LCD_load_multiple_chars(const char Customs_CGRAM[][8], uint8_t addr_nmbr) {

    if(addr_nmbr > 8) {
        addr_nmbr = 8;          // CGRAM safety net, can not store > 8 custom chars
    }

    uint8_t i = 0;
    for(i = 0; i < addr_nmbr; i++) {
        LCD_custom_char(i, Customs_CGRAM[i]);
    }

}
