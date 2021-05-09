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
#include "timer.h"
#endif

void set_PWM(double frequency) {

		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) {OCR3A = 0xFFFF; }
		else if (frequency > 31250) {OCR3A = 0x0000;}
		else {OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		
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

enum states {wait, press, hold} state;
unsigned char finished = 1;
unsigned char currNote = 0;


void TickFCT_PushButton(unsigned char tmpA) {
	switch (state) {
		case wait:
			if (tmpA == 0x01) {
				state = press;

			}
			break;
		case press:
			state = hold;			
			break;
		case hold:
			if (tmpA == 0x01) {
				state = hold;		
			} else {
				state = wait;			
			}
			break;
	}	
	
	switch (state) {
		case press:
			finished = 0;
			break;
		case wait:
			if (finished){
				set_PWM(0);			
			}
			break;
		case hold:
			if (finished){
				set_PWM(0);			
			}
			break;
		default:
			break;
	}
}


void TickFCT_Melody(unsigned char tmpA, double frequencies[], double holdTimes[]) {
	if (!finished) {
		set_PWM(frequencies[currNote]);
		TimerSet(holdTimes[currNote]);
		currNote++;
		if (currNote >= 7) {
			currNote = 0;
			finished = 1;		
		} 
	}
}





int main(void) {
	
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	TimerSet(100);
	TimerOn();	

	double frequencies[7] = {261.63, 329.63, 523.25, 493.88, 523.25, 493.88, 523.25};
	double holdTimes[7] = {400, 400, 1000, 1000, 1000, 1000, 400};


	currNote = 0;
	

	unsigned char tmpA;
	
	PWM_on();
	while (1) {
		tmpA = ~PINA;
		TickFCT_PushButton(tmpA);
		TickFCT_Melody(tmpA, frequencies, holdTimes);
		
		while (!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}

