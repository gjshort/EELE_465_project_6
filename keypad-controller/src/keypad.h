/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the header file for all keypad
            functionality
    
 **************************************************************/

#include <msp430fr2310.h>
#include <stdint.h>
#include "msp2310_utils.h"

 #pragma once

/**
*   Struct for matrix keypad pins 
*/
typedef struct {

    msp2310_port     R1;
    msp2310_port     R2;
    msp2310_port     R3;
    msp2310_port     R4;
    msp2310_port     C1;
    msp2310_port     C2;
    msp2310_port     C3;

} keypad_interface;

/**
* Struct for keypad led indicator
*/
typedef struct {

    msp2310_port    keypad_blink;

} keypad_led;

// -------------------------------------------------- IMPLICIT FUNCTION DECLARATIONS -----------------------------------------
void keypad_column_init();
void keypad_set_input(msp2310_port *p);
void keypad_set_high(msp2310_port *p);
uint8_t keypad_column_pressed(msp2310_port *keypad_col);
char poll_keypad();
void keypad_led_toggle();
void keypad_led_init();
