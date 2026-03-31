/**************************************
* Author:   Gabe Story
* Date:     03.30.2026
* Class:    EELE 465
* Purpose:  This is the c file for the rotary encoder
            functionality

************************************************************************/

#include <msp430fr2153.h>
#include <stdint.h>
#include <stdbool.h>
#include "LCD_Driver.h"
#include "rotary_encoder.h"

/**
* Bool statements for rotary encoder rotation
* and button press
*/
bool rotary_CW     = false;
bool rotary_CCW    = false;
bool rotary_switch = false;

/**
* A & B channel variables for polling 
*/
static uint8_t A_state_last = 1;
uint8_t         A_state_current;
uint8_t         B_state_current;

/**
*  Map rotary interface to msp430fr2153 GPIO
*/
rotary_interface rotary_pins = {

    .A  = {&P3DIR, &P3REN, &P3OUT, &P3IN, BIT0},
    .B  = {&P3DIR, &P3REN, &P3OUT, &P3IN, BIT1},
    .sw = {&P1DIR, &P1REN, &P1OUT, &P1IN, BIT6}

};


/**
* Helper function to set rotary pins A & B
* to input with pull-ups
*/
void rotary_pins_input_up(rotary_port *p) {

    *(p->port_dir) &= ~(p->bit_mask);
    *(p->port_ren) |=  (p->bit_mask);
    *(p->port_out) |=  (p->bit_mask);

}

/**
* Helper function to set rotary pins 1 & 2
* to input with pull-down
*/
void rotary_pins_input_dwn(rotary_port *p) {

    *(p->port_dir) &= ~(p->bit_mask);
    *(p->port_ren) |=  (p->bit_mask);
    *(p->port_out) &= ~(p->bit_mask);

}

/**
* Init A and B channels of rotary_encoder
*/
void init_rotary_A_B() {

    rotary_pins_input_up(&rotary_pins.A);
    rotary_pins_input_up(&rotary_pins.B);

}

/**
* Init switch of rotary_encoder 
*/
void init_rotary_sw() {

    rotary_pins_input_dwn(&rotary_pins.sw);

}

/**
* So the logic following the A & B chaneels
* of the rotary encoder is if you turn CW
* A = 0, then after a delay B = 1
* and vice-versa for CCW
*/

/**
* So the idea is anytime we poll channel A
* we check what channel B is and determine
* rotation of rotary encoder and assign
* dedicated outputs
*/
void poll_rotary_rotation(uint8_t cursor_col, uint8_t *cursor_row) {

    /**
    * First we have to store the A & B channel states by reading
    * the whole port and bit masking so we read only desired bit
    */
    
    uint8_t A_state_old;
    uint8_t B_state_old;

    A_state_old = *(rotary_pins.A.port_in);
    B_state_old = *(rotary_pins.B.port_in);

    A_state_old = A_state_old & rotary_pins.A.bit_mask;
    B_state_old = B_state_old & rotary_pins.B.bit_mask;

    /**
    * Assign integer values to polled bit masks
    */
    if(A_state_old == 0) {
        A_state_current = 0;
    } else {
        A_state_current = 1;
    }

    if(B_state_old == 0) {
        B_state_current = 0;
    } else {
        B_state_current = 1;
    }

    /**
    * Coonditional IF output statement
    */
    if((A_state_last == 0) && (A_state_current == 1)) {

        if(B_state_current == 1) {          // We just polled A, so if we went CW B channel should be a 1, 
            rotary_CW  = true;              // meaning we move the cursor down the LCD screen
            rotary_CCW = false;
        } else {
            rotary_CCW = true;              // In the case B channel is a 0, that means we went CCW,
            rotary_CW  = false;             // meaning we move the cursor up the LCD screen
        }

    }

    A_state_last = A_state_current;         // Save previous poll value for next poll

}

/**
* Poll rotary switch 
*/
void poll_rotary_switch() {

    uint8_t switch_state;

    switch_state = *(rotary_pins.sw.port_in);

    switch_state = switch_state & rotary_pins.sw.bit_mask;


    if(switch_state != 0) {
        rotary_switch = true;
    }

}

/**
* Init clock to 16MHz
*/
void init_CLK() {

    FRCTL0 = FRCTLPW | NWAITS_1;
    __bis_SR_register(SCG0);

    CSCTL3 = SELREF__REFOCLK;
    CSCTL1 = DCORSEL_5;
    CSCTL2 = FLLD_0 + 487;
    __delay_cycles(3);

    __bic_SR_register(SCG0);
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;

}
