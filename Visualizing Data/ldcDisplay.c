/*
 *       lcdDisplay.c
 *
 *   Created on:  November 10, 2017
 *  Last Edited:  November 10, 2017
 *       Author:  Nick Gorab
 *        Board:  6989
 */





/************************************************************************************************\
*                                                                                               *
*  IF YOU'RE USING THIS CODE AS REFERENCE AND HAVE QUESTIONS, FEEL FREE TO SLACK ME @nickgorab  *
*                                                                                               *
\************************************************************************************************/





#include <msp430.h>
#include "LCDDriver.h"



/************************\
 *                      *
 *   Code Definitions   *
 *                      *
\************************/

int Ones = 0;
int Tens = 0;
int Hundreds = 0;
int ADC_Data = 0;



/************************\
 *                      *
 *  LCD Initialization  *
 *                      *
\************************/

void lcdInit(void){
  PJSEL0      = BIT4 | BIT5;                               // Sets pins for LCD Display
  LCDCPCTL0   = 0xFFFF;                                    // Initializes the LCD Segments
  LCDCPCTL1   = 0xFC3F;                                    // Initializes the LCD Segments
  LCDCPCTL2   = 0x0FFF;                                    // Initializes the LCD Segments
  LCDCCTL0    = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;
  LCDCVCTL    = VLCD_1 | VLCDREF_0 | LCDCPEN;
  LCDCCPCTL   = LCDCPCLKSYNC;                              // Clock synchronization enabled
  LCDCMEMCTL  = LCDCLRM;                                   // Clear LCD memory
  LCDCCTL0   |= LCDON;
}


/************************\
 *                      *
 *  DCO Initialization  *
 *                      *
\************************/

void clkInit(void){
  CSCTL0_H = CSKEY_H;       // Unlocks clock registers for writing
  CSCTL1   = DCOFSEL_3      // Sets DCO at 8MHz
           | DCORSEL;       // Sets DCO at 8MHz
  CSCTL2   = SELA__VLOCLK   // Selects ACLK source of VLOCLK
           | SELS__DCOCLK   // SMCLK source as DCOCLK
           | SELM__DCOCLK;  // MCLK source as DCOCLK
  CSCTL3   = DIVA__1        // ACLK source divider of 2
           | DIVS__1        // SMCLK source divider of 2
           | DIVM__1;       // MCLK source divider of 2
  CSCTL0_H = 0;             // Locks clock registers
}



/************************\
 *                      *
 *  ADC Initialization  *
 *                      *
\************************/

void adcInit(void){
    P8SEL0    |= BIT7;          // ADC readings taken on A4 (Pin 1.1)
    ADC12MCTL0  = ADC12INCH_4;  // ADC input channel 4 (A4)
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
    TA0CCTL0 = CCIE;      // Emables Timer_A interrupts
    TA0CTL   = TASSEL_1   // Uses SMCLK
             + MC_1;      // Counts in Up-Mode
    TA0CCR0  = 12800;     // Samples ~ every second
}



/***********************************\
 *                                 *
 *  Display Conditioning Function  *
 *                                 *
\***********************************/

void formatAndDisplay(int rawSauce){
    rawSauce = rawSauce/34;            // Normalizes temp data (OPAmp Gain of 2.5)
    Ones     = (rawSauce % 10);        // Determines ones place
    Tens     = ((rawSauce/10) % 10);   // Determines tens place
    Hundreds = ((rawSauce/100) % 10);  // Determines hundreds place
    showChar(Ones, 0);                 // Displays ones digit
    showChar(Tens, 1);                 // Displats tens digit
    showChar(Hundreds, 2);             // Displats hundreds digit
}


/***********************\
 *                     *
 *    Main Function    *
 *                     *
\***********************/

void main(void){
    WDTCTL = WDTPW+WDTHOLD;                 // Stops Watchdog Timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disables high impedance mode
    clkInit();                              // Initializes DCO
    adcInit();                              // Initializes ADC
    timerInit();                            // Initializes TIMER_A

    while(1){                               // Infinite loop for constant updating
        formatAndDisplay(ADC_Data);         // Formats the temp data and displays it.
        __bis_SR_register(LPM0_bits + GIE); // Enter LPM0, Enable interrupts
    }
}



/************************\
 *                      *
 *  TIMER_A Interrupt   *
 *                      *
\************************/

#pragma vector = TIMER0_A0_VECTOR         // Timer_A interrupt
__interrupt void Timer_A (void) {         // Timer_A interrupt vector function decleration
    ADC_Data = ADC12MEM0;                 // Stores ADC memory
    __bic_SR_register_on_exit(LPM0_bits); // Exits LPM0
}