/*
 *       MSP430FR2311_ADC.c
 *
 *   Created on:  November 1, 2017
 *  Last Edited:  November 2, 2017
 *       Author:  Nick Gorab
 *        Board:  2311
 */





/************************************************************************************************\
*                                                                                               *
*  IF YOU'RE USING THIS CODE AS REFERENCE AND HAVE QUESTIONS, FEEL FREE TO SLACK ME @nickgorab  *
*                                                                                               *
\************************************************************************************************/





#include <msp430.h>



/************************\
 *                      *
 *   Code Definitions   *
 *                      *
\************************/

unsigned int TX_Data = 0;    // 16-bit integer
         char MSB    = 0;    // 8-bit integer
         char LSB    = 0;    // 8-bit integer



/************************\
 *                      *
 *  CLK Initialization  *
 *                      *
\************************/

void clkInit(void){
    __bis_SR_register(SCG0);      // Disables FLL
      CSCTL3 |= SELREF__REFOCLK;  // Set REFO as FLL reference source
      CSCTL1  = DCOFTRIMEN_1      // DCO Trim
              | DCOFTRIM0         // DCO Trim
              | DCOFTRIM1         // DCO Trim
              | DCORSEL_3;        // 8MHz DCO Range
      CSCTL2  = FLLD_0 + 243;     // DCODIV = 8MHz
      __delay_cycles(3);          // Delay for set-up time
      __bic_SR_register(SCG0);    // Enables FLL
      CSCTL4  = SELMS__DCOCLKDIV  // Sets REFO as ACLK source
              | SELA__REFOCLK;    // Sets REFO as ACLK source
}



/*************************\
 *                       *
 *  UART Initialization  *
 *                       *
\*************************/

void uartInit(void) {
    P1SEL0 |= BIT7;                         // UART TX
    UCA0CTLW0 |= UCSWRST;                   // State machine reset
    UCA0CTLW0 |= UCSSEL__SMCLK;             // Uses SMCLK as source
    UCA0BR0 = 52;                           // Modulation
    UCA0BR1 = 0x00;                         // Modulation
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;  // Modulation
    UCA0CTLW0 &= ~UCSWRST;                  // Starts state machine
}



/************************\
 *                      *
 *  ADC Initialization  *
 *                      *
\************************/

void adcInit(void) {
    P1SEL0 |= BIT1;               // // Sets up Pin 1.1 for ADC
    P1SEL1 |= BIT1;               // Sets up Pin 1.1 for ADC
    ADCCTL0 |= ADCSHT_2 | ADCON;  // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP             // Sampling signal comes from sampling timer
            | ADCCONSEQ_2;        // Sets ADC for continuous sampling
    ADCCTL2 |= ADCRES;            // Sets resolution to 10 bits
    ADCMCTL0 |= ADCINCH_1;        // Selects A1 as ADC input (Pin 1.1)
}



/***************************\
 *                         *
 * TIMER_A  Initialization *
 *                         *
\***************************/

void timerInit(void) {
    TB0CCTL0 = CCIE;       // Emables Timer_B interrupts
    TB0CTL   = TBSSEL_2    // Uses SMCLK
             + MC_1        // Counts in Up-Mode
             + ID_3;       // Predivider of 8
    TB0CCR0  = 32000;      // Samples ~ every second
}



/********************************\
 *                              *
 *  Data Conditioning Function  *
 *                              *
\********************************/

void formatAndSend(int value) {
    MSB       = value >> 8;      // Bit Shifts 12 bits to the right by 8
    LSB       = value & 0xFF;    // ANDs the 12 bit value with 11111111, returning the LSB
    UCA0TXBUF = MSB;             // Transmits the MSB first
    while(!(UCA0IFG & UCTXIFG)); // Waits for the TX buffer to be cleared
    UCA0TXBUF = LSB;             // Transmits the LSB second
}



/***********************\
 *                     *
 *    Main Function    *
 *                     *
\***********************/

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;     // Stop Watchdog Timer
    PM5CTL0 &= ~LOCKLPM5;         // Disables high impedance mode
    clkInit();                    // Initializes 8MHz clock
    adcInit();                    // Initializes ADC
    uartInit();                   // Initializes UART
    timerInit();                  // Initializes TIMER_B
    __bis_SR_register(GIE);       // Enables interrupts
    while(1){                     // While loop for sampling 
      ADCCTL0 |= ADCENC | ADCSC;  // Sampling and conversion star
    }
}



/************************\
 *                      *
 *  TIMER_A Interrupt   *
 *                      *
\************************/

#pragma vector = TIMER0_B0_VECTOR     // Timer_B interrupt
__interrupt void Timer_B (void) {     // Timer_B interrupt vector function decleration
    TX_Data = ADCMEM0;                // Stores ADC memory
    formatAndSend(TX_Data);           // Formats it into bytes, and sends it.
}
