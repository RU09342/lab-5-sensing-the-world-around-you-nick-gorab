/*
 *       MSP430F5529_ADC.c
 *
 *   Created on:  November 6, 2017
 *  Last Edited:  November 6, 2017
 *       Author:  Nick Gorab
 *        Board:  2553
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
  DCOCTL = 0;             // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;  // Set DCO at 1 MHz
  DCOCTL = CALDCO_1MHZ;   // Sets DCO at 1 MHz
}

/*************************\
 *                       *
 *  UART Initialization  *
 *                       *
\*************************/

void uartInit(void) {
    P1SEL    |= BIT1 + BIT2 + BIT5; // P1.1 = RXD, P1.2=TXD
    P1SEL2   |= BIT1 + BIT2;        // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;           // SMCLK
    UCA0BR0   = 104;                // 1MHz 9600
    UCA0BR1   = 0;                  // 1MHz 9600
    UCA0MCTL  = UCBRS0;             // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;           // **Initialize USCI state machine**
}




/************************\
 *                      *
 *  ADC Initialization  *
 *                      *
\************************/

void adcInit(void){
    ADC10CTL0  = ADC10ON        // Turns on ADC12
               + ADC10SHT_2     // Sets sampling time
               + MSC;           // Sets up multiple sample conversion
    ADC10CTL1  = INCH_0         // Signal comes from sampling timer instead of input
               + CONSEQ_2;      // Repeats reading on single channel
    ADC10CTL0 |= ENC            // Enables conversions
              |  ADC10SC;       // Starts conversions
}



/***************************\
 *                         *
 * TIMER_A  Initialization *
 *                         *
\***************************/

void timerInit(void){
    CCTL0 = CCIE;         // Emables Timer_A interrupts
    TACTL   = TASSEL_2    // Uses SMCLK
             + MC_1;      // Counts in Up-Mode
    TA0CCR0  = 10000;     // Samples ~ every second
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
    while(!(IFG2 & UCA0TXIFG));  // Waits for the TX buffer to be cleared
    UCA0TXBUF = LSB;             // Transmits the LSB second
}



/***********************\
 *                     *
 *    Main Function    *
 *                     *
\***********************/

void main(void){
    WDTCTL = WDTPW+WDTHOLD;             // Stops Watchdog Timer
    clkInit();                          // Initializes DCO
    uartInit();                         // Initializes UART
    adcInit();                          // Initializes ADC
    timerInit();                        // Initializes TIMER_A
    __bis_SR_register(LPM0_bits + GIE); // Enter LPM4, Enable interrupts
}



/************************\
 *                      *
 *  TIMER_A Interrupt   *
 *                      *
\************************/

#pragma vector = TIMER0_A0_VECTOR       // Timer_A interrupt
__interrupt void Timer_A (void) {       // Timer_A interrupt vector function decleration
    TX_Data = ADC10MEM;                 // Stores ADC memory
    formatAndSend(TX_Data);             // Formats it into bytes, and sends it.
}
