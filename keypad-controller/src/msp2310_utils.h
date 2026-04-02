/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the header file for msp430fr2310 controller
            utilities
    
 **************************************************************/

#include <msp430fr2310.h>
#include <stdint.h>

#pragma once

/**
* Struct for pin dir/out and bitmask for keypad, uART and LED(s)
*/
typedef struct {

    volatile    uint8_t *port_sel1;     // pointer to PxSEL1
    volatile    uint8_t *port_sel0;     // pointer to PxSEL0
    volatile    uint8_t *port_dir;      // pointer to PxDIR
    volatile    uint8_t *port_ren;      // poimter to PxREN
    volatile    uint8_t *port_out;      // pointer to PxOUT
    volatile    uint8_t *port_in;       // pointer to PxIN
                uint8_t  bit_mask;      // BITx select

} msp2310_port;

// --------------------------------------- IMPLICIT FUNCTION DECLARATIONS -----------------------------------------------
void init_CLK();
