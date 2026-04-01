/****************************************
*  Author:  Gabe Story
*    Date:  03.31.2026
*   Class:  EELE 465
* Purpose:  This is the test file for uART sending
            keypad scans from msp2310 to msp2153

**************************************************************/

#include <msp430fr2153.h>
#include <stdint.h>
#include "utils.h"
#include "eUSCI.h"
#include "LCD_Driver.h"

int main(void)
{
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Init
    init_CLK();
    init_eUSCI_A1_uart();
    LCD_init_4bit();
    LCD_clear();

    // Enable IE
    UCA1IE |= UCRXIE;
    __enable_interrupt();

    // Disable low-power mode
    PM5CTL0 &= ~LOCKLPM5;

    while(1) {}

}

#pragma vector = EUSCI_A1_VECTOR
__interrupt void ISR_EUSCI_A1(void) {

    LCD_type_char(UCA1RXBUF);
    
}
