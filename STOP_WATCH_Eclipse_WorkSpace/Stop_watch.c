/*
 * Stop_watch.c
 *
 *  Created on: Apr 17, 2023
 *      Author: Doha
 */
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

#define NO_COMPARE_MATCHES 15	//Number of compare matches per 1 second

	unsigned char g_tick=0;		//counter of compare matches

	//tow counters for each: seconds, minutes , hours
	unsigned char counter1_sec=0;
	unsigned char counter2_sec=0;

	unsigned char counter1_min=0;
	unsigned char counter2_min=0;

	unsigned char counter1_h=0;
	unsigned char counter2_h=0;

void delay_display(void);	//delay 2 seconds between displaying each 7segment (round robin technique)
void Increment_digits(unsigned char *first, unsigned char *second);

/*reset stop watch counters when falling edge is applied to interrupt 0*/
ISR(INT0_vect)
{
	 counter1_sec=0;
	 counter2_sec=0;

	 counter1_min=0;
	 counter2_min=0;

	  counter1_h=0;
	  counter2_h=0;
}
void INT0_Init(void)
{
    MCUCR |=(1<<ISC01);	//interrupt with the falling edge
    GICR|=(1<<6);//interrupt0 enable
}

/*Pause stop watch by stopping clock source when rising edge is applied to interrupt 1*/
ISR(INT1_vect)
{
	TCCR1B	&= ~(1<<CS10) & ~(1<<CS11) & ~(1<<CS12);//No clock source
}
void INT1_Init(void)
{
    MCUCR |=(1<<ISC10)|(1<<ISC11);	//interrupt with the rising
    GICR|=(1<<7);//interrupt1 enable
}

/*resume stop watch by restarting clock source when falling edge is applied to interrupt 2 */
ISR(INT2_vect)
{
	TCCR1B	|=(1<<CS10);//Enable timer with No prescaling
}
void INT2_Init(void)
{
    MCUCSR &=~(1<<ISC2);	//interrupt with the falling edge
    GICR|=(1<<5);//interrupt2 enable
}



ISR( TIMER1_COMPA_vect)
{
	g_tick++;	//increment number of compare matches
	if(g_tick==NO_COMPARE_MATCHES)	//One second is counted
	{

		Increment_digits(&counter1_sec,&counter2_sec);
		g_tick=0;
	}

}
//enable timer1 CTC mode
void TIMER1_COMPA_Init(void)
{
	TCNT1=0;			//counter start
	OCR1A=65000;
	TCCR1A |=(1<<FOC1A);
	TCCR1B |=(1<<WGM12)|(1<<CS10);	//CTC mode No prescaling
	TIMSK=(1<<OCIE1A);	//Enable Output Compare A Match Interrupt
	SREG |=(1<<7); 		//Enable I bit
}
int main(void)
{		//interrupts enable in ATmega
		DDRD &=~(1<<2)& ~(1<<3);	//configure pin 2(INT0),3(INT1) in port D as an input pins
		DDRB &=~(1<<2);			//configure pin 2(INT2) in port B as an input pin
		PORTD |=(1<<2);//enable pull up internal circuit
		PORTB |=(1<<2);//enable pull up internal circuit

		//7 segments enable/disable in ATmega
		DDRA |=0x3F;	//configure first 6 pins in port A as an output pins

		//Decoder enable in ATmega
		DDRC |= 0x0F;	//configure the first 4 pins in port C as an output pins
		PORTC &= 0xF0;	//ALL leds are off;

		TIMER1_COMPA_Init();//start timer1
		INT0_Init();		//enable Interrupt0
		INT1_Init();		//enable Interrupt1
		INT2_Init();		//enable Interrupt2
		while(1)
		{
			delay_display();	//keep displaying stop watch counters
		}
}
void delay_display(void)/*One 7 segment is active and displaying its counter every 2 ms */
{
	PORTA &=0xC0;	// All 7segments are off
	PORTA |=0x01;	//first 7 segment is only active
	PORTC = (PORTC & 0xF0)| (counter1_sec & 0x0F);
	_delay_ms(2);

	PORTA &=0xC0;
	PORTA |=0x02;	//second 7 segment is only active
	PORTC = (PORTC & 0xF0)| (counter2_sec & 0x0F);
	_delay_ms(2);

	PORTA &=0xC0;
	PORTA |=0x04;	//third 7 segment is only active
	PORTC = (PORTC & 0xF0)| (counter1_min & 0x0F);
	_delay_ms(2);

	PORTA &=0xC0;
	PORTA |=0x08;	//fourth 7 segment is only active
	PORTC = (PORTC & 0xF0)| (counter2_min & 0x0F);
	_delay_ms(2);

	PORTA &=0xC0;
	PORTA |=0x10;	//fifth 7 segment is only active
	PORTC = (PORTC & 0xF0)| (counter1_h & 0x0F);
	_delay_ms(2);

	PORTA &=0xC0;
	PORTA |=0x20;	//sixth 7 segment is only active
	PORTC = (PORTC & 0xF0)| (counter2_h & 0x0F);
	_delay_ms(2);

}

void Increment_digits(unsigned char *first, unsigned char *second)
{
	if(*first<9)
	{
		(*first)++;	//increment the first digit of seconds 1--> 9
	}
	else
	{
		*first=0;	//reached the maximum value 9 of the fist digit so reset the first digit
		if(*second<5)
		{
			(*second)++;//second digit is incremented
		}
		else
		{
			/*when second digit reached it maximum (60 second or 60 minutes)*/
			if(counter2_sec>=5)	// seconds counters reached 60 second therefore minutes must be incremented
			{
				*second=0;		//reset the second digit of seconds
				Increment_digits(&counter1_min,&counter2_min);	//minutes must be incremented
			}
			else if(counter2_min>=5)// minutes counters reached 60 minutes therefore hours must be incremented
			{
				*second=0;			//reset the second digit of minutes
				Increment_digits(&counter1_h,&counter2_h);		//hours must be incremented
			}else if(counter2_h>=5)
			{
				counter2_h++;		//the second digit of hours is not limited it keeps incrementing 60 hours 70 hours ..etc
			}

		}
	}

}
