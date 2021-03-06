/*
 *       MSP430F5529_ADC.c
 *
 *   Created on:  November 7, 2017
 *  Last Edited:  November 7, 2017
 *       Author:  Nick Gorab
 *        Board:  5994
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

#define LM35        0x00
#define PhotoDiode  0x01
#define Thermistor  0x02 


unsigned int  TX_Data = 0;    // 16-bit integer
         int  sensor  = 0;    // Determines sensor conversions
         char MSB     = 0;    // 8-bit integer
         char LSB     = 0;    // 8-bit integer



/************************\
 *                      *
 *  DCO Initialization  *
 *                      *
\************************/

void dcoInit(void){
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

/*************************\
 *                       *
 *  UART Initialization  *
 *                       *
\*************************/

void uartInit(void) {
    P6SEL0 |= BIT0;          // UART TX
    UCA3CTLW0 |= UCSWRST;    // State machine reset
    UCA3CTLW0 |= UCSSEL1;    // Uses SMCLK as source
    UCA3BRW    = 52;         // Modulation
    UCA3MCTLW  = UCBRF_1     // Modulation
              | UCOS16       // Modulation
              | 0x4900;      // Modulation
    UCA3CTLW0 &= ~UCSWRST;   // Starts state machine
}




/************************\
 *                      *
 *  ADC Initialization  *
 *                      *
\************************/

void adcInit(void){
    P1SEL0    |= BIT2;          // ADC readings taken on A2 (Pin 1.2)
    P1SEL1    |= BIT2;          // ADC readings taken on A2 (Pin 1.2)
    ADC12MCTL0 = ADC12INCH_2;   // Selects input channel 2 (A2)
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
    TA0CCR0  = 6400;      // Samples ~ every second
}



/********************************\
 *                              *
 *  Data Conditioning Function  *
 *                              *
\********************************/

void formatAndSend(int value){
    switch(sensor){
      case 0:             // LM35 Sensor
        value = value/34;
      break;
      case 1:             // Photodiode
      break;
      case 2:             // Thermistor
      break;
    }
    MSB       = value >> 8;      // Bit Shifts 12 bits to the right by 8
    LSB       = value & 0xFF;    // ANDs the 12 bit value with 11111111, returning the LSB
    UCA0TXBUF = MSB;             // Transmits the MSB first
    while(!(UCA3IFG & UCTXIFG)); // Waits for the TX buffer to be cleared
    UCA3TXBUF = LSB;             // Transmits the LSB second
}



/***********************\
 *                     *
 *    Main Function    *
 *                     *
\***********************/

void main(void){
    WDTCTL = WDTPW+WDTHOLD;                 // Stops Watchdog Timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disables high-impedance mode
    dcoInit();                              // Initializes DCO
    uartInit();                             // Initializes UART
    adcInit();                              // Initializes ADC
    timerInit();                            // Initializes TIMER_A

    while(1){                               // Infinite loop for constant readings
        formatAndSend(TX_Data);             // Formats it into bytes, and sends it.
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
    TX_Data = ADC12MEM0;                  // Stores ADC memory
    __bic_SR_register_on_exit(LPM0_bits); // Exits LPM0
}
