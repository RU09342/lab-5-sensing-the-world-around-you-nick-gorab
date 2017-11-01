/*
 *       MSP430G2553_ADC.c
 *
 *   Created on:  November 1, 2017
 *  Last Edited:  November 1, 2017
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

int TX_Data = 0;    // 16-bit integer
char MSB    = 0;    // 8-bit integer
char LSB    = 0;    // 8-bit integer



/*************************\
 *                       *
 *  UART Initialization  *
 *                       *
\*************************/

void uartInit(void) {
    P2SEL    |=  BIT5       // UART TX
    UCA0CTL1 |=  UCSWRST    // Resets state machine
             |   UCSSEL_2;  // SMCLK
    UCA0BR0   =  6;         // 9600 Baud Rate
    UCA0BR1   =  0;         // 9600 Baud Rate
    UCA0MCTL |=  UCBRS_0    // Modulation
             |   UCBRF_13   // Modulation
             |   UCOS16;    // Modulation
    UCA0CTL1 &= ~UCSWRST;   // Initializes the state machine
    UCA0IE   |=  UCRXIE;    // Enables USCI_A0 RX Interrupt
}



/************************\
 *                      *
 *  ADC Initialization  *
 *                      *
\************************/

void adcInit(void){
    P1SEL   |= BIT5;          // ADC readings taken on A5 (Pin 1.5)
    ADCCTL0  = ADCON        // Turns on ADC12
             + ADCSHT0_8    // Sets sampling time
             + ADCMSC;      // Sets up multiple sample conversion
    ADCCTL1  = ADCSHP       // Signal comes from sampling timer instead of input
             + ADCCONSEQ_2; // Repeats reading on single channel
    ADCCTL0 |= ADCENC       // Enables conversions
            |  ADCSC;       // Starts conversions
}



/***************************\
 *                         *
 * TIMER_A  Initialization *
 *                         *
\***************************/

void timerInit(void){
    TB0CCTL0 = CCIE;        // Emables Timer_B interrupts
    TB0CTL   = TBSSEL_1     // Uses SMCLK
             + MC_1;        // Counts in Up-Mode
    TB0CCR0  = 32700;       // Samples ~ every second
}



/********************************\
 *                              *
 *  Data Conditioning Function  *
 *                              *
\********************************/

void formatAndSend(int value){
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

void main(void){
    WDTCTL = WDTPW+WDTHOLD;             // Stops Watchdog Timer
    uartInit();                         // Initializes UART
    adcInit();                          // Initializes ADC
    timerInit();                        // Initializes TIMER_B
    __bis_SR_register(LPM4_bits + GIE); // Enter LPM4, Enable interrupts
}



/************************\
 *                      *
 *  TIMER_A Interrupt   *
 *                      *
\************************/

#pragma vector = TIMER0_B0_VECTOR       // Timer_B interrupt
__interrupt void Timer_B (void) {       // Timer_B interrupt vector function decleration
    TX_Data = ADC12MEM0;                // Stores ADC memory
    formatAndSend(TX_Data);             // Formats it into bytes, and sends it.
}
