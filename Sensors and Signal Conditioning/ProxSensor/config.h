/*
 * config.h
 *
 *   Created on:  September 30, 2017
 *  Last Edited:  October 1, 2017
 *       Author:  Nick Gorab
 *        Board:  ALL
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef __MSP430FR5994__
    #define BUTTON1     BIT5
    #define BUTTON2     BIT6
    #define LED1        BIT0
    #define LED2        BIT1
    #define B1_DIR      P5DIR
	#define B1_IN		P5IN
    #define B1_REN      P5REN
    #define B1_OUT      P5OUT
    #define B1_IE       P5IE
    #define B1_IES      P5IES
    #define B1_IFG      P5IFG
    #define L1_DIR      P1DIR
    #define L1_OUT      P1OUT
    #define B2_DIR      P5DIR
    #define B2_REN      P5REN
    #define B2_OUT      P5OUT
    #define B2_IE       P5IE
    #define B2_IES      P5IES
    #define B2_IFG      P5IFG
    #define L2_DIR      P1DIR
    #define L2_OUT      P1OUT
    #define HIGHZ       (PM5CTL0 &= ~LOCKLPM5)
#endif

#ifdef __MSP430FR2311__
    #define BUTTON1     BIT1
    #define LED1        BIT0
    #define LED2        BIT0
    #define B1_DIR      P1DIR
	#define B1_IN		P1IN
    #define B1_REN      P1REN
    #define B1_OUT      P1OUT
    #define B1_IE       P1IE
    #define B1_IES      P1IES
    #define B1_IFG      P1IFG
    #define L1_DIR      P1DIR
    #define L1_OUT      P1OUT
    #define L2_DIR      P2DIR
    #define L2_OUT      P2OUT
    #define HIGHZ       (PM5CTL0 &= ~LOCKLPM5)
#endif

#ifdef __MSP430G2553__
    #define BUTTON1     BIT3
    #define LED1        BIT0
    #define LED2        BIT6
    #define B1_DIR      P1DIR
	#define B1_IN		P1IN
    #define B1_REN      P1REN
    #define B1_OUT      P1OUT
    #define B1_IE       P1IE
    #define B1_IES      P1IES
    #define B1_IFG      P1IFG
    #define L1_DIR      P1DIR
    #define L1_OUT      P1OUT
    #define L2_DIR      P1DIR
    #define L2_OUT      P1OUT
    #define HIGHZ       0
#endif

#ifdef __MSP430F5529__
    #define BUTTON1     BIT1
    #define BUTTON2     BIT1
    #define LED1        BIT0
    #define LED2        BIT7
    #define B1_DIR      P1DIR
	#define B1_IN		P1IN
    #define B1_REN      P1REN
    #define B1_OUT      P1OUT
    #define B1_IE       P1IE
    #define B1_IES      P1IES
    #define B1_IFG      P1IFG
    #define L1_DIR      P1DIR
    #define L1_OUT      P1OUT
    #define B2_DIR      P2DIR
    #define B2_REN      P2REN
    #define B2_OUT      P2OUT
    #define B2_IE       P2IE
    #define B2_IES      P2IES
    #define B2_IFG      P2IFG
    #define L2_DIR      P4DIR
    #define L2_OUT      P4OUT
    #define HIGHZ       0
#endif

#ifdef __MSP430FR6989__
    #define BUTTON1     BIT1
    #define BUTTON2     BIT2
    #define LED1        BIT0
    #define LED2        BIT7
    #define B1_DIR      P1DIR
	#define B1_IN		P1IN
    #define B1_REN      P1REN
    #define B1_OUT      P1OUT
    #define B1_IE       P1IE
    #define B1_IES      P1IES
    #define B1_IFG      P1IFG
    #define L1_DIR      P1DIR
    #define L1_OUT      P1OUT
    #define B2_DIR      P1DIR
    #define B2_REN      P1REN
    #define B2_OUT      P1OUT
    #define B2_IE       P1IE
    #define B2_IES      P1IES
    #define B2_IFG      P1IFG
    #define L2_DIR      P9DIR
    #define L2_OUT      P9OUT
    #define HIGHZ       (PM5CTL0 &= ~LOCKLPM5)
#endif

#endif /* CONFIG_H_ */
