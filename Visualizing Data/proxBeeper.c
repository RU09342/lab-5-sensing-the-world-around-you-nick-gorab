/*
 *       proxBeeper.c
 *
 *   Created on:  November 7, 2017
 *  Last Edited:  November 7, 2017
 *       Author:  Nick Gorab
 *        Board:  5529
 */





/************************************************************************************************\
*                                                                                               *
*  IF YOU'RE USING THIS CODE AS REFERENCE AND HAVE QUESTIONS, FEEL FREE TO SLACK ME @nickgorab  *
*                                                                                               *
\************************************************************************************************/





#include <msp430.h>

/************************\
 *                      *
 *  ADC Initialization  *
 *                      *
\************************/

void adcInit(void){
    P6SEL     |= BIT0;          // ADC readings taken on A0 (Pin 6.0)
    ADC12CTL0  = ADC12ON        // Turns on ADC12
               + ADC12SHT0_8    // Sets sampling time
               + ADC12MSC;      // Sets up multiple sample conversion
    ADC12CTL1  = ADC12SHP       // Signal comes from sampling timer instead of input
               + ADC12CONSEQ_2; // Repeats reading on single channel
    ADC12CTL0 |= ADC12ENC       // Enables conversions
              |  ADC12SC;       // Starts conversions
}



/***************************\
 *                         *
 * TIMER_A  Initialization *
 *                         *
\***************************/

void timerInit(void){
    TA0CCTL0 = CCIE;
    TA0CCR0 = 10000;
    TA0CTL   = TASSEL_2 + MC_1 + ID_3;  // SMCLK, Up-Mode, Predivider 8
    TA0EX0 = TAIDEX_7;
    TA1CCTL0 = CCIE;
    TA1CTL = TASSEL_2 + MC_1;
    TA1CCR0 = 32000;


}

void ledInit(void){
    P1DIR |=  BIT2;       // P1.2 to output
}



/***********************\
 *                     *
 *    Main Function    *
 *                     *
\***********************/

void main(void){
    WDTCTL = WDTPW+WDTHOLD;       // Stops Watchdog Timer
    adcInit();                    // Initializes ADC
    timerInit();                  // Initializes PWM
    ledInit();                    // Initializes LED
    __bis_SR_register(LPM0_bits + GIE);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
    P1OUT ^= BIT2;     // Toggles the LED on Pin 1.0 via XOR
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1 (void) {
    TA0CCR0 = 10000 - (ADC12MEM0*2);
}