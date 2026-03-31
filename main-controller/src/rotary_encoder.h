/**************************************
* Author:   Gabe Story
* Date:     03.30.2026
* Class:    EELE 465
* Purpose:  This is the header file for the rotary encoder
            functionality

************************************************************************/

#include <msp430fr2153.h>
#include <stdint.h>
#include <stdbool.h>

#pragma once

/**
* Let main know these bad boys are in another c file 
*/
extern bool     rotary_CW;
extern bool    rotary_CCW;
extern bool rotary_switch;

/**
* Generic struct for port use
*/
typedef struct {

    volatile    uint8_t *port_dir;      // pointer to PxDIR
    volatile    uint8_t *port_ren;      // pointer to PxREN
    volatile    uint8_t *port_out;      // pointer to PxOUT
    volatile    uint8_t  *port_in;      // pointer to PxIN
                uint8_t  bit_mask;      // BITx select

} rotary_port;

/**
* Struct for specific rotary encoder ports
*/
typedef struct {

    rotary_port  A;
    rotary_port  B;
    rotary_port sw;

} rotary_interface;

// -------------------------------------------------- IMPLICIT FUNCTION DECLARATIONS -----------------------------------------
void rotary_pins_input_up(rotary_port *p);
void rotary_pins_input_dwn(rotary_port *p);
void init_rotary_A_B();
void init_rotary_sw();
void init_CLK();
void poll_rotary_rotation(uint8_t cursor_col, uint8_t *cursor_row);
void poll_rotary_switch();
