/*
 * Timer1.c
 *
 * Created: 02.12.2022 08:24:29
 *  Author: Students
 */ 


#include "Timer1.h"

#include <avr/io.h>

unsigned short uszaehler = 0;						//Variable zum Z�hlen der Microsekunden
unsigned char ucFlag_Tim = 0;						//Timer-Flag, die zeigt, ob die ISR auf die steigende oder fallende Flanke reagieren soll 

void Timer1_Init(void)								//Initialisierung des Timers1
{
	TCCR1B |= 1<<WGM12;								//CTC-Modus										
	TIMSK1 |= 1 << OCIE1A;							//Timer Interrupt wird freigegeben
	sei();											//Freigabe aller Interrupts
}

void EI_Init()										//Initialisierung des externen Interrupts
{
	EICRA |= (1 << ISC01) | (1 << ISC00);			//ISR wird bei steigender Flanke aufgerufen
	EIMSK |= (1 << INT0);							//Externer Interrupt 0 wird freigeschalten
}

ISR(INT0_vect)										//Interrupt-Service-Routine (wird bei einer Flanke am externen Interrupt 0 aufgerufen)
{
	if(ucFlag_Tim == 0)								//Falls auf die steigende Flanke reagiert werden soll
	{  
		TCNT1 = 0;									//Timerwert zur�cksetzen
		TCCR1B |= (1 << CS12);						//Prescaler 256
		EICRA &= ~(1 << ISC00);						//ISR wird als n�chstes auf fallende Flanke ausgel�st
		ucFlag_Tim = 1;								//Flag setzen, sodass auf die fallende Flanke reagiert wird
	}
	else											//andernfalls (es soll auf fallende Flanke reagiert werden)
	{
		uszaehler = TCNT1;							//Variable uizaehler wird die Timerzahl, bis zu der gez�hlt wurde, zugewiesen
		TCCR1B &= ~(1 <<CS12);						//Prescaler = 0;
		EICRA |= (1 << ISC00);						//ISR wird nun entsprechend wieder bei der steigenden Flanke ausgel�st
		ucFlag_Tim = 0;								//Flag zur�cksetzen, sodass auf die steigend Flanke reagiert wird
	}
}

