/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the header file for heartbeat
            functionality
    
 **************************************************************/

 #include <msp430fr2310.h>
 #include <stdint.h>
 #include "msp2310_utils.h"

 #pragma once

/**
* Struct for heartbeat LED
*/
typedef struct {

    msp2310_port blink;

} heartbeat_interface;

// --------------------------------------- IMPLICIT FUNCTION DECLARATIONS -----------------------------------------------
void heartbed_set_output(msp2310_port *p);
void heartbeat_xor(msp2310_port *p);
void init_heartbeat_led_2310();
void init_msp2310_timer();
void heartbeat_toggle();
