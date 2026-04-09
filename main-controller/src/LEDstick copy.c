/***********************************
* Author:   Gabe Story
* Date:     02.26.2026
* Class:    EELE 465
* Purpose:  This c file will hold all the functions needed
            to operate the WS2812B LED stick

            We will be able to set the color for individual
            pixels, and change brightness, as well as map LED
            pixels to potentiometer.
**********************************************************************/

#include    <msp430fr2153.h>
#include    <math.h>
#include    <stdint.h>
#include    "LEDstick.h"

// Struct for LED color
typedef struct {

    u_char  green;
    u_char  red;
    u_char  blue;

} GRB;

static GRB ledStick[PixNumber] = { {0, 0, 0} };

unsigned int LEDpot;

// Set the color for an individual pixel
void setColor(u_int n, u_char g, u_char r, u_char b) {

    ledStick[n].green = g;
    ledStick[n].red   = r;
    ledStick[n].blue  = b;

}

// Send color to the Stick 
void sendStick() {

    //__bic_SR_register(GIE);                         // disable interrupts

    // Send GRB color to each pixel
    unsigned int i, j;
    for(i = 0; i < PixNumber; i++) {
        u_char *grb = (u_char *) & ledStick[i];     // get grb color for i-th pixel

        // Send green values, red values, then blue values
        for(j = 0; j < 3; j++) {
            u_char mask = 0x80;                     // 0b1000000

            // check each bit
            while(mask != 0) {
                while(!(UCA0IFG & UCTXIFG));        // wait to tx
                if(grb[j] & mask) {
                    UCA0TXBUF = highTide;           // send 1
                } else {
                    UCA0TXBUF = lowTide;            // send 0
                }
                mask >>= 1;                         // check next bit
            }
        }
    }

    __delay_cycles(800);            // send reset signal
    //__bis_SR_register(GIE);

}

// Clear the Stick, turn all pixels off
void clearStick() {

    StickFiller(0x00, 0x00, 0x00);

}

// Populate whole Stick with one color
void StickFiller(u_char g, u_char r, u_char b) {

    int i;
    for(i = 0; i < PixNumber; i++) {
        setColor(i, g, r, b);
    }
    sendStick();
}


void potStick(uint16_t LEDpot, u_char g, u_char r, u_char b) {

        /* This if/else statement reads voltage on pin 6
           and then changes the compare value, 
           lighting up a pixel */
        TB0CTL &= ~MC__STOP;    // Stop timer
        if(LEDpot <= 50) {
            setColor(0, (g = 0), (r = 0), (b = 0));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 32768;
        } else if(LEDpot <= 100) {
            setColor(0, (g/8), (r/8), (b/8));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 32768;
        } else if(LEDpot <= 150) {
            setColor(0, (g/7), (r/7), (b/7));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 32768;
        } else if(LEDpot <= 200) {
            setColor(0, (g/6), (r/6), (b/6));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 32768;
        } else if(LEDpot <= 250) {
            setColor(0, (g/5), (r/5), (b/5));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 32768;
        } else if(LEDpot <= 300) {
            setColor(0, (g/4), (r/4), (b/4));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 30933;
        } else if(LEDpot <= 350) {
            setColor(0, (g/3), (r/3), (b/3));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 30933;
        } else if(LEDpot <= 400) {
            setColor(0, (g/2), (r/2), (b/2));
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 30933;
        } else if(LEDpot <= 450) {
            setColor(0, g, r, b);
            setColor(1, 0, 0, 0);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 30933;
        } else if(LEDpot <= 500) {
            setColor(0, g, r, b);
            setColor(1, (g/8), (r/8), (b/8));
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 30933;
        } else if(LEDpot <= 550) {
            setColor(0, g, r, b);
            setColor(1, (g/7), (r/7), (b/7));
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 29163;
        } else if(LEDpot <= 600) {
            setColor(0, g, r, b);
            setColor(1, (g/6), (r/6), (b/6));
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 29163;
        } else if(LEDpot <= 650) {
            setColor(0, g, r, b);
            setColor(1, (g/5), (r/5), (b/5));
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 29163;
        } else if(LEDpot <= 700) {
            setColor(0, g, r, b);
            setColor(1, (g/4), (r/4), (b/4));
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 29163;
        } else if(LEDpot <= 750) {
            setColor(0, g, r, b);
            setColor(1, (g/3), (r/3), (b/3));
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 29163;
        } else if(LEDpot <= 800) {
            setColor(0, g, r, b);
            setColor(1, (g/2), (r/2), (b/2));
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 27197;
        } else if(LEDpot <= 850) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, 0, 0, 0);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 27197;
        } else if(LEDpot <= 900) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, (g/8), (r/8), (b/8));
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 27197;
        } else if(LEDpot <= 950) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, (g/7), (r/7), (b/7));
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 27197;
        } else if(LEDpot <= 1000) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, (g/6), (r/6), (b/6));
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 27197;
        } else if(LEDpot <= 1050) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, (g/5), (r/5), (b/5));
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 25559;
        } else if(LEDpot <= 1100) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, (g/4), (r/4), (b/4));
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 25559;
        } else if(LEDpot <= 1150) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, (g/3), (r/3), (b/3));
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 25559;
        } else if(LEDpot <= 1200) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, (g/2), (r/2), (b/2));
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 25559;
        } else if(LEDpot <= 1250) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, 0, 0, 0);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 25559;
        } else if(LEDpot <= 1300) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, (g/8), (r/8), (b/8));
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 23593;
        } else if(LEDpot <= 1350) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, (g/7), (r/7), (b/7));
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 23593;
        } else if(LEDpot <= 1400) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, (g/6), (r/6), (b/6));
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 23593;
        } else if(LEDpot <= 1450) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, (g/5), (r/5), (b/5));
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 23593;
        } else if(LEDpot <= 1500) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, (g/4), (r/4), (b/4));
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 23593;
        } else if(LEDpot <= 1550) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, (g/3), (r/3), (b/3));
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 21627;
        } else if(LEDpot <= 1600) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, (g/2), (r/2), (b/2));
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 21627;
        } else if(LEDpot <= 1650) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, 0, 0, 0);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 21627;
        } else if(LEDpot <= 1700) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, (g/8), (r/8), (b/8));
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 21627;
        } else if(LEDpot <= 1750) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, (g/7), (r/7), (b/7));
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 21627;
        } else if(LEDpot <= 1800) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, (g/6), (r/6), (b/6));
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 19988;
        } else if (LEDpot <= 1850) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, (g/5), (r/5), (b/5));
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 19988;
        }

       //--------------------------------------- //

        
        else if(LEDpot <= 1900) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, (g/4), (r/4), (b/4));
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);   
            setColor(9, 0, 0, 0); 
            TB0CCR0 = 19988;
        }  else if(LEDpot <= 1950) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, (g/3), (r/3), (b/3));
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 19988;
        } else if(LEDpot <= 2000) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, (g/2), (r/2), (b/2));
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 19988;
        } else if(LEDpot <= 2050) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, 0, 0, 0);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 19988;
        } else if(LEDpot <= 2100) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, (g/8), (r/8), (b/8));
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 18022;
        } else if(LEDpot <= 2150) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, (g/7), (r/7), (b/7));
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 18022;
        } else if(LEDpot <= 2200) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, (g/6), (r/6), (b/6));
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 18022;
        } else if(LEDpot <= 2250) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, (g/5), (r/5), (b/5));
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 18022;
        } else if(LEDpot <= 2300) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, (g/4), (r/4), (b/4));
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 16384;
        } else if(LEDpot <= 2350) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, (g/3), (r/3), (b/3));
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 16384;
        } else if(LEDpot <= 2400) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, (g/2), (r/2), (b/2));
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 16384;
        } else if(LEDpot <= 2450) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, 0, 0, 0);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 16384;
        } else if(LEDpot <= 2500) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, (g/8), (r/8), (b/8));
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 16384;
        } else if(LEDpot <= 2550) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, (g/7), (r/7), (b/7));
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 16384;
        } else if(LEDpot <= 2600) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, (g/6), (r/6), (b/6));
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 14418;
        } else if(LEDpot <= 2650) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, (g/5), (r/5), (b/5));
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 14418;
        } else if(LEDpot <= 2700) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, (g/4), (r/4), (b/4));
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 14418;
        } else if(LEDpot <= 2750) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, (g/3), (r/3), (b/3));
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 14418;
        } else if(LEDpot <= 2800) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, (g/2), (r/2), (b/2));
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 14418;
        } else if(LEDpot <= 2850) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, 0, 0, 0);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 12452;
        } else if(LEDpot <= 2900) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, (g/8), (r/8), (b/8));
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 12452;
        } else if(LEDpot <= 2950) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, (g/7), (r/7), (b/7));
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 12452;
        } else if(LEDpot <= 3000) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, (g/6), (r/6), (b/6));
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 12452;
        } else if(LEDpot <= 3050) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, (g/5), (r/5), (b/5));
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 12452;
        } else if(LEDpot <= 3100) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, (g/4), (r/4), (b/4));
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 10813;
        } else if(LEDpot <= 3150) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, (g/3), (r/3), (b/3));
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 10813;
        } else if(LEDpot <= 3200) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, (g/2), (r/2), (b/2));
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 10813;
        } else if(LEDpot <= 3250) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, 0, 0, 0);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 10813;
        } else if(LEDpot <= 3300) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, (g/8), (r/8), (b/8));
            setColor(9, 0, 0, 0);
            TB0CCR0 = 10813;
        } else if(LEDpot <= 3350) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, (g/7), (r/7), (b/7));
            setColor(9, 0, 0, 0);
            TB0CCR0 = 8847;
        } else if(LEDpot <= 3400) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, (g/6), (r/6), (b/6));
            setColor(9, 0, 0, 0);
            TB0CCR0 = 8847;
        } else if(LEDpot <= 3450) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, (g/5), (r/5), (b/5));
            setColor(9, 0, 0, 0);
            TB0CCR0 = 8847;
        } else if(LEDpot <= 3500) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, (g/4), (r/4), (b/4));
            setColor(9, 0, 0, 0);
            TB0CCR0 = 8847;
        } else if(LEDpot <= 3550) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, (g/3), (r/3), (b/3));
            setColor(9, 0, 0, 0);
            TB0CCR0 = 8847;
        } else if(LEDpot <= 3600) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, (g/2), (r/2), (b/2));
            setColor(9, 0, 0, 0);
            TB0CCR0 = 7209;
        } else if(LEDpot <= 3650) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, 0, 0, 0);
            TB0CCR0 = 7209;
        } else if (LEDpot <= 3700) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, (g/8), (r/8), (b/8));
            TB0CCR0 = 7209;
        } else if(LEDpot <= 3750) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, (g/7), (r/7), (b/7));
            TB0CCR0 = 7209;
        } else if(LEDpot <= 3800) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, (g/6), (r/6), (b/6));
            TB0CCR0 = 7209;
        } else if(LEDpot <= 3850) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, (g/5), (r/5), (b/5));
            TB0CCR0 = 5243;
        } else if(LEDpot <= 3900) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, (g/4), (r/4), (b/4));
            TB0CCR0 = 5243;
        } else if(LEDpot <= 3950) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, (g/3), (r/3), (b/3));
            TB0CCR0 = 5243;
        } else if(LEDpot <= 4000) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, (g/2), (r/2), (b/2));
            TB0CCR0 = 5243;
        } else if(LEDpot <= 4050) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, g, r, b);
            TB0CCR0 = 5243;
        } else if (LEDpot <= 4100) {
            setColor(0, g, r, b);
            setColor(1, g, r, b);
            setColor(2, g, r, b);
            setColor(3, g, r, b);
            setColor(4, g, r, b);
            setColor(5, g, r, b);
            setColor(6, g, r, b);
            setColor(7, g, r, b);
            setColor(8, g, r, b);
            setColor(9, g, r, b);
            TB0CCR0 = 3277;
        }
        TB0CTL |= MC__UP;      // Restart timer
        
    sendStick();
        
}

/* References:   
I want to cite these sources for inspiration & useful how-to
that helped me get started.
               
                - Mjmeli: MSP430 | NeoPixel library https://github.com/mjmeli/MSP430-NeoPixel-WS2812-Library/tree/master
*/
