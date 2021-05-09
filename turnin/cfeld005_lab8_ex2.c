/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) {OCR3A = 0xFFFF; }
		else if (frequency > 31250) {OCR3A = 0x0000;}
		else {OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
	}	
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);

	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


short currNote;
enum states2 {wait, inc, dec, holdInc, holdDec} SM2_State;
enum states1 {pressOn, holdOn, waitOn, pressOff, holdOff, waitOff} SM1_State;



void TickFCT_Toggle(unsigned char tmpA){
	switch(SM1_State){
		case waitOn:
			if (tmpA & 0x04) {
				SM1_State = pressOff;			
			}
			break;
		case pressOff:
			if (tmpA & 0x04) {
				SM1_State = holdOff;			
			}
			break;
		case holdOff:
			if (tmpA & 0x04) {
				SM1_State = holdOff;			
			} else {
				SM1_State = waitOff;			
			}
			break;
		case waitOff:
			if (tmpA & 0x04) {
				SM1_State = pressOn;			
			}
			break;
		case pressOn:
			if (tmpA & 0x04) {
				SM1_State = holdOn;			
			}
			break;
		case holdOn:
			if (tmpA & 0x04) {
				SM1_State = holdOn;			
			} else {
				SM1_State = waitOn;			
			}
			break;
	}
	switch(SM1_State) {
		case pressOn:
			PWM_on();
			break;
		case pressOff:
			PWM_off();
			break;	
	}			
}


void TickFCT_C_Scale(unsigned char tmpA, double frequencies[]){
	switch(SM2_State) {
		case wait:
			if (tmpA == 0x01) {
				SM2_State = inc;			
			} else if (tmpA == 0x02) {
				SM2_State = dec;
			} else {
				SM2_State = wait;			
			}
			break;
		case inc:
			SM2_State = holdInc;			
			break;
		case dec:
			SM2_State = holdDec;
			break;
		case holdInc:
			if (tmpA == 0x01) {
				SM2_State = holdInc;			
			} else {
				SM2_State = wait;
			}
			break;
		case holdDec:
			if (tmpA == 0x02) {
				SM2_State = holdDec;			
			} else {
				SM2_State = wait;
			}
			break;

	}

	switch(SM2_State) {
		case inc: 
			if (currNote < 8){
				currNote++;	
				set_PWM(frequencies[currNote]);			
			}	
			break;
		case dec: 
			if (currNote > 0){
				currNote--;
				set_PWM(frequencies[currNote]);					
			}
			break;
		default:
			break;
	}	
}

int main(void) {
	
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	double frequencies[9] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};

	SM1_State = waitOn;
	SM2_State = wait;

	currNote = 0;
	

	unsigned char tmpA;
	
	PWM_on();
	while (1) {
		tmpA = ~PINA;
		TickFCT_Toggle(tmpA);
		TickFCT_C_Scale(tmpA, frequencies);
	}
	PWM_off();
	return 1;
}

