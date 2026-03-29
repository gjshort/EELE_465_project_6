/************************************
* Author:   Gabe Story
* Date:     02.22.2026
* Class:    EELE 465
* Purpose:  Policy code for sampling ADC to control
            period of the LED patterns
****************************************************/

#include <msp430fr2153.h>
#include <stdbool.h>
#include <stdint.h>
#include "Potentiometer.h"

ADCpot a;

// Initialize ADC to use A1
void init_ADCpot() {

    // P1.1 | pin 6 for A1
    P1SEL1 |= BIT1;
    P1SEL0 |= BIT1;

    // Configure ADC
    ADCCTL0 &= ~ADCSHT;
    ADCCTL0 |= ADCSHT_2;        // conversion cycles = 16
    ADCCTL0 |= ADCON;

    ADCCTL1 |= ADCSHP;          //Sample signal comes from sample timer
    ADCCTL1 |= ADCSSEL_3;       //Use SMCLK (16 MHz) - divided by 32
    ADCCTL1 |= ADCDIV_7;        // Divide by 8

    ADCCTL2 &= ~ADCRES;         //Clear resolution
    ADCCTL2 |= ADCRES_2;        //Set to 12 bit resolution
    ADCCTL2 |= ADCPDIV_1;       //Predivide by 4

    ADCMCTL0 |= ADCINCH_1;      // ADC input channel to A1

}

// Read potentiometer voltage, change TB0CCR0
void ADCpot_period(uint16_t LEDpot) {                      

        a.ADCpot_value = LEDpot;

        /* This if/else statement reads voltage on pin 6
           and then changes the compare value, 
           changing the period of the LED toggling */
        TB0CTL &= ~MC__STOP;    // Stop timer
        if(a.ADCpot_value <= 256) {
            TB0CCR0 = 32768;
        } else if(a.ADCpot_value <= 512) {
            TB0CCR0 = 30933;
        } else if(a.ADCpot_value <= 768) {
            TB0CCR0 = 29163;
        } else if(a.ADCpot_value <= 1024) {
            TB0CCR0 = 27197;
        } else if(a.ADCpot_value <= 1280) {
            TB0CCR0 = 25559;
        } else if(a.ADCpot_value <= 1536) {
            TB0CCR0 = 23593;
        } else if(a.ADCpot_value <= 1792) {
            TB0CCR0 = 21627;
        } else if(a.ADCpot_value <= 2048) {
            TB0CCR0 = 19988;
        } else if(a.ADCpot_value <= 2304) {
            TB0CCR0 = 18022;
        } else if(a.ADCpot_value <= 2560) {
            TB0CCR0 = 16384;
        } else if(a.ADCpot_value <= 2816) {
            TB0CCR0 = 14418;
        } else if(a.ADCpot_value <= 3072) {
            TB0CCR0 = 12452;
        } else if(a.ADCpot_value <= 3328) {
            TB0CCR0 = 10813;
        } else if(a.ADCpot_value <= 3584) {
            TB0CCR0 = 8847;
        } else if(a.ADCpot_value <= 3840) {
            TB0CCR0 = 7209;
        } else if(a.ADCpot_value <= 4096) {
            TB0CCR0 = 5243;
        } else if(a.ADCpot_value >= 4096) {
            TB0CCR0 = 3277;
        }
        TB0CTL |= MC__UP;      // Restart timer

}
