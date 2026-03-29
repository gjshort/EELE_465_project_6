/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the header file for msp430fr2310 uART
    
 **************************************************************/

#include <msp430fr2310.h>
#include <stdint.h>
#include "msp2310_utils.h"

#pragma once

/**
* Struct for msp2310 uART
*/
typedef struct {

    msp2310_port tx_2310;
    msp2310_port rx_2310;

} uART_interface;

// --------------------------------------- IMPLICIT FUNCTION DECLARATIONS -----------------------------------------------
void init_msp2310_uart();
void init_msp2310_tx_rx();
void init_msp2310_uart_pins(msp2310_port *p);
