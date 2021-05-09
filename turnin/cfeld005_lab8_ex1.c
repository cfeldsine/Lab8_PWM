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

enum states {wait, p1, p2, p3} state;

void TickFCT_3Notes(unsigned char tmpA){
	switch(state) {
		case wait:
			if (tmpA == 0x01) {
				state = p1;			
			} else if (tmpA == 0x02) {
				state = p2;			
			} else if (tmpA == 0x04) {
				state = p3;			
			} else {
				state = wait;			
			}
			break;
		case p1: 
			if (tmpA == 0x01) {
				state = p1;			
			} else if (tmpA == 0x02) {
				state = p2;			
			} else if (tmpA == 0x04) {
				state = p3;			
			} else {
				state = wait;			
			}
			break;
		case p2:
			if (tmpA == 0x01) {
				state = p1;			
			} else if (tmpA == 0x02) {
				state = p2;			
			} else if (tmpA == 0x04) {
				state = p3;			
			} else {
				state = wait;			
			}
			break;
		case p3:
			if (tmpA == 0x01) {
				state = p1;			
			} else if (tmpA == 0x02) {
				state = p2;			
			} else if (tmpA == 0x04) {
				state = p3;			
			} else {
				state = wait;			
			}
			break;
		default:
			break;	
	}

	switch (state) {
		case wait:
			PWM_off();
			break;
		case p1:
			set_PWM(261.63);
			PWM_on();
			break;
		case p2:
			PWM_on();
			set_PWM(293.66);
			break;
		case p3:
			PWM_on();
			set_PWM(329.63);
			break;
		default:
			break;			
	}
}


int main(void) {
	
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	state = wait;
	unsigned char tmpA;

	PWM_on();
	/* Insert your solution below */
	while (1) {
		tmpA = ~PINA;
		TickFCT_3Notes(tmpA);
	}
	PWM_off();
	return 1;
}
