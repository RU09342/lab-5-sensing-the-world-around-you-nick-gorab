/*
 *       MSP430F5529_ADC.c
 *
 *   Created on:  October 31, 2017
 *  Last Edited:  October 31, 2017
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
 *   Code Definitions   *
 *                      *
\************************/

unsigned int TX_Data = 0;    // 16-bit integer
         char MSB    = 0;    // 8-bit integer
         char LSB    = 0;    // 8-bit integer



void clkInit(void){
  CSCTL0_H = CSKEY >> 8;
  CSCTL1   = DCOFSEL_3
           | DCORSEL;
  CSCTL2   = SELA__VLOCLK
           | SELS__DCOCLK
           | SELM__DCOCLK;
  CSCTL3   = DIVA__1
           | DIVS__1
           | DIVM__1;
  CSCTL0_H = 0;
}

/*************************\
 *                       *
 *  UART Initialization  *
 *                       *
\*************************/

void uartInit(void) {
    P2SEL0    |=  BIT0       // UART TX
              |   BIT1;      // UART RX
    UCA0CTL1  |=  UCSWRST    // Resets state machine
              |   UCSSEL_2;  // SMCLK
    UCA0BR0    =  52;        // 9600 Baud Rate
    UCA0BR1    =  0x00;         // 9600 Baud Rate
    UCA0MCTLW  =  0x4900    // Modulation
              |   UCBRF_1   // Modulation
              |   UCOS16;    // Modulation
    UCA0CTLW0 &= ~UCSWRST;   // Initializes the state machine
    UCA0IE    |=  UCRXIE;    // Enables USCI_A0 RX Interrupt
}



/************************\
 *                      *
 *  ADC Initialization  *
 *                      *
\************************/

void adcInit(void){
    P8SEL0    |= BIT7;          // ADC readings taken on A1 (Pin 1.1)
    ADC12MCTL0  = ADC12INCH_4;
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
    TA0CCTL0 = CCIE;        // Emables Timer_A interrupts
    TA0CTL   = TASSEL_2     // Uses SMCLK
             + MC_1;        // Counts in Up-Mode
    TA0CCR0  = 32700;       // Samples ~ every second
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
    timerInit();                        // Initializes TIMER_A
    __bis_SR_register(LPM4_bits + GIE); // Enter LPM4, Enable interrupts
}



/************************\
 *                      *
 *  TIMER_A Interrupt   *
 *                      *
\************************/

#pragma vector = TIMER0_A0_VECTOR       // Timer_A interrupt
__interrupt void Timer_A (void) {       // Timer_A interrupt vector function decleration
    TX_Data = ADC12MEM0;                // Stores ADC memory
    formatAndSend(TX_Data);             // Formats it into bytes, and sends it.
}
