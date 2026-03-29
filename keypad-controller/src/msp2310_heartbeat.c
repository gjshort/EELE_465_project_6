/****************************************
*  Author:  Gabe Story
*    Date:  03.27.2026
*   Class:  EELE 465
* Purpose:  This is the c file for msp430fr2310
            heartbeat led
    
 **************************************************************/

 #include <msp430fr2310.h>
 #include <stdint.h>
 #include "msp2310_utils.h"
 #include "msp2310_heartbeat.h"

heartbeat_interface led_pin = {

    .blink = {&P2SEL1, &P2SEL0, &P2DIR, &P2REN, &P2OUT, &P2IN, BIT6}

};

/**
* Set heartbeat led as output, turned off
*/
void heartbeat_set_output(msp2310_port *p) {

    *(p->port_dir) |=  (p->bit_mask);
    *(p->port_out) &= ~(p->bit_mask);

}

/**
* Toggle heartbeat output
*/
void heartbeat_xor(msp2310_port *p) {

    *(p->port_out) ^= (p->bit_mask);

}

/**
* Init heartbeat led for msp2310
*/
void init_heartbeat_led_2310() {

    heartbeat_set_output(&led_pin.blink);

}

/** 
*Turn heartbeat on and off
*/
inline void heartbeat_toggle() {

    heartbeat_xor(&led_pin.blink);

}

/**
* Init timer
*/
void init_msp2310_timer() {

    // Configure Timer
    TB0CTL |= TBCLR;
    TB0CTL |= TBSSEL__ACLK;    // Clear TimerB0, select ACLK, and use the UP counter
    TB0CTL |= MC__UP;

    TB0CCR0 = 32768;

    // Setup Interrupts
    TB0CCTL0 &= ~CCIFG;         // Clear CCR0 Flag
    TB0CCTL0 |= CCIE;           // Enable CCR0 IRQ

}
