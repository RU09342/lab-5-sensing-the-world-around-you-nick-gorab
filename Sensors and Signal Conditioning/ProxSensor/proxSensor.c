/*
 * DistanceSensor.c
 *
 *   Created on:  October 18, 2017
 *  Last Edited:  October 18, 2017
 *       Author:  Nick Gorab
 *		  Board:  5529
 */

#include <msp430.h>
#include <config.h>

void configureADC(void){
	ADC12CTL0 = ADC12INCH_5 + ADC12DIV_3;
}