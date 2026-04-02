/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the c file for all keypad
            functionality
    
 **************************************************************/

 #include <msp430fr2310.h>
 #include <stdint.h>
 #include "msp2310_utils.h"
 #include "keypad.h"

/**
* Map keypad pins to msp430fr2310 GPIO
*/
keypad_interface keypad_pins = {

    .R1 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT1},
    .R2 = {&P2SEL1, &P2SEL0, &P2DIR, &P2REN, &P2OUT, &P2IN, BIT0},
    .R3 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT5},
    .R4 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT3},
    .C1 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT2},
    .C2 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT0},
    .C3 = {&P1SEL1, &P1SEL0, &P1DIR, &P1REN, &P1OUT, &P1IN, BIT4}

};

keypad_led keypad_led_pin = {

    .keypad_blink = {&P2SEL1, &P2SEL0, &P2DIR, &P2REN, &P2OUT, &P2IN, BIT7}

};

/** 
* Assign ASCII values to a lookup table
*/
static const char keypad_ascii_values[4][3] = {

    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}

};

static msp2310_port *keypad_row[4] = {

    &keypad_pins.R1,
    &keypad_pins.R2,
    &keypad_pins.R3,
    &keypad_pins.R4


};

static msp2310_port *keypad_column[3] = {

    &keypad_pins.C1,
    &keypad_pins.C2,
    &keypad_pins.C3

};

/**
* keypad_set_high, keypad_set_low, and keypad_set_input used to
* configure rows and columns. Since we are using a matrix keypad
* I made the choice to set the rows constant high, columns
* as inputs and poll a column switch to check for keypress
*/

/** 
* Set pins ouput high, for rows of matrix
*/
void keypad_set_high(msp2310_port *p) {

    *(p->port_dir) |= (p->bit_mask);
    *(p->port_out) |= (p->bit_mask);

}

/** 
* Set pins ouput low, for rows of matrix
*/
void keypad_set_low(msp2310_port *p) {

    *(p->port_dir) |=  (p->bit_mask);
    *(p->port_out) &= ~(p->bit_mask);

}

/**
* Set keypad inputs w/ pull-down resistor (used for columns)
*/
void keypad_set_input(msp2310_port *p) {

    *(p->port_dir) &= ~(p->bit_mask);
    *(p->port_ren) |=  (p->bit_mask);
    *(p->port_out) &= ~(p->bit_mask);

}

/**
* Toggle keypad indicator led
*/
void keypad_xor(msp2310_port *p) {

    *(p->port_out) ^= (p->bit_mask);

}

void keypad_column_init() {

    keypad_set_input(&keypad_pins.C1);
    keypad_set_input(&keypad_pins.C2);
    keypad_set_input(&keypad_pins.C3);

}

void keypad_led_init() {

    keypad_set_low(&keypad_led_pin.keypad_blink);

}

/**
* Function to indicate keypress
*/
void keypad_led_toggle() {


    keypad_xor(&keypad_led_pin.keypad_blink);       // turn on
    __delay_cycles(1000000);                        // delay to see LED toggle
    keypad_xor(&keypad_led_pin.keypad_blink);       // turn off

}

/** 
* So the idea is to poll for a key press
* that tells us a column has been pulled high.
* This helper function will check for us if a
* column gets pulled high.
*/
uint8_t keypad_column_pressed(msp2310_port *keypad_col) {

    return(*(keypad_col->port_in) & (keypad_col->bit_mask)) != 0;

}

/**
* Using the lookup table and function above
* we can create our pool function to return
* ASCII value when polling individual switches 
* in the matrix.
*/

char poll_keypad() {

    uint8_t i = 0;          // i for indexing rows
    uint8_t j = 0;          // j for indexing columns

    // Set all rows in matrix low
    for(i = 0; i < 4; i++) {
        keypad_set_low(keypad_row[i]);
    }

    // Send a row high one at a time and scan for columns driving high
    for(i = 0; i < 4; i++) {
        
        keypad_set_high(keypad_row[i]);

        for(j = 0; j < 3; j++) {
            if(keypad_column_pressed(keypad_column[j])) {
                keypad_led_toggle();
                keypad_set_low(keypad_row[i]);
                return(keypad_ascii_values[i][j]);
            }
        }
        
        keypad_set_low(keypad_row[i]);

    }

    // If no key is pressed
    return(0);

}
