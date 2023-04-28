/***************************************
*                                     *
*            Stop Watch               *
*         By: Aya Mohsen              *
*   Created on: 10th of April, 2023   *
*            Atmega328P
***************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Global variables initialization: Volatile used in ISR
//using Timer1: CTC mode
volatile uint8_t second = 0;
volatile uint8_t minute = 0;
volatile uint8_t hour =0;


void Timer1_CTC(void){
	TCCR1A |= (1<<COM1A1); // clear on compare mode
	TCCR1B |= (1<<WGM12) | (1<<CS12) ; //CTC, Prescaler = 256
	TCCR1C |= (1<<FOC1A);
	OCR1A=15624; //1s= 1 Hz, OCR = 8MHz/2(256*1) -1 
	SREG |= (1<<I); //enable interrupt
	TIMSK |= (1<<OCIE1A); //enable compare match interrupt
}

ISR(TIMER1_COMPA_vect){
	second++;
	if (second ==60){
		second = 0;
		minute++;
	}
	if (minute == 60){
		second = 0;
		minute = 0;
		hour ++;
	}
	if (hour == 12){
		second = 0;
		minute = 0;
		hour = 0;
	}
}

//RESET
void INT0_Init_Reset(void){
	DDRD &= ~(1<<PD2); //input
	PORTD |= (1<<PD2); // pull-up
	EICRA |= (1<<ISC01); //Falling edge
	EIMSK |= (1<<INT0); //enable external interrupt 0
}
ISR(INT0_vect){
	second = 0;
	minute = 0;
	hour = 0;
}

//PAUSE
void INT1_Init_Pause(void){
	DDRD &= ~(1<<PD3); //input
	PORTD |= (1<<PD3); //pullup
	EICRA |= (1<<ISC10) | (1<<ISC11);//Rising edge
	EIMSK |= (1<<INT1); //enable external interrupt 1
}

ISR(INT1_vect){
	TIMSK &= ~(1<<OCIE1A);
}

//RESUME
void PCINT0_Init_Resume(void){
	DDRB &= ~(1<<PB0); //input
	PORTB |= (1<<PB0); //pullup
	PCICR |= (1<<PCIE0); //enable pin change interrupt enable 0
	PCMSK0 |= (1<<PCINT0); //enable bin 0 in PCINT0
}
ISR(PCINT0_vect){
	TIMSK |= (1<<OCIE1A);
}


//Main program
int main(void){
	DDRC = 0b00001111;
	PORTC = 0X00; //set to low
	DDRB = Ob01111110;
	PORTB = 0x11; //set to high
	SREG|=(1<<I); //Enable I-bit
	
	INT0_Init_Reset();
	INT1_Init_Pause();
	PCINT0_Init_Resume();
	Timer1_CTC();
	
	while(1)
	{
		PORTB=(1<<PB6); //6th 7-segment
		PORTC=(second%10); //1st digit
		_delay_ms(5);
		PORTB=(1<<PB5); //5th 7-segment
		PORTC=(second/10); //2nd digit
		_delay_ms(5);
		PORTB=(1<<PB4); //4th 7-segment
		PORTC=(minute%10); //1st digit
		_delay_ms(5);
		PORTB=(1<<PB3); //3rd 7-segment
		PORTC=(minute/10); //2nd digit
		_delay_ms(5);
		PORTB=(1<<PB2); //2nd 7-segment
		PORTC=(hour%10);  //1st digit
		_delay_ms(5);
		PORTB=(1<<PB1); //1st 7-segment
		PORTC=(hour/10);  //2nd digit
		_delay_ms(5);
	}
}