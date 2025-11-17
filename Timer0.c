/*
 * Timer0.c
 *
 * Created: 29.04.2022 10:46:37
 *  Author: Students
 */ 

#include "Timer0.h"


unsigned char ucFlag10ms = 0, ucFlag1s = 0;						//Flags, die gesetzt werden sobald die entsprechende Zeit verstrichen ist
unsigned short uiCnt1s = 0;										//Z�hler, um die Flags zur richtigen Zeit zu setzen

void Timer0_Init(void)											//Initialisierung des Timers0
{
	TCCR0A |= 1<<WGM01;											// CTC-Modus
	TCCR0B |= (1 << CS00) | (1<<CS02);							// Prescaler 1024
	OCR0A = 179;												// 10 ms @ 18432000 Hz
	TIMSK0 |= 1 << OCIE0A;										// Timer Interrupt wird freigegeben
	sei();														// Freigabe aller Interrupts
}

ISR(TIMER0_COMPA_vect)											//Interrupt-Service-Routine (wird alle 10ms aufgerufen)
{
	ucFlag10ms = 1;												//Flag f�r 10ms wird gesetzt
	uiCnt1s++;													//Z�hler f�r 1s wird erh�ht
	if(uiCnt1s == 100)											//1s ist vergangen
	{
		uiCnt1s = 0;											//Z�hler f�r 1s wird auf 0 gesetzt
		ucFlag1s = 1;											//Flag f�r 1s wird gesetzt
	}
}

unsigned char Timer0_get_10msState(void)						//Abfrage des Timer-Status f�r 10ms
{
	if(ucFlag10ms == 1)											//Falls das Flag f�r 10ms gesetzt ist
	{
		ucFlag10ms = 0;											//Flag f�r 10ms wird zur�ckgesetzt
		return TIMER_TRIGGERED;									//R�ckgabe, dass der Timer durchlaufen ist
	}
	return TIMER_RUNNING;										//R�ckgabe, dass der Timer noch l�uft
}

unsigned char Timer0_get_1sState(void)							//Abfrage des Timer-Status f�r 1s
{
	if(ucFlag1s == 1)											//Falls das Flag f�r 1s gesetzt ist
	{
		ucFlag1s = 0;											//Flag f�r 1s wird zur�ckgesetzt
		return TIMER_TRIGGERED;									//R�ckgabe, dass der Timer durchlaufen ist
	}
	return TIMER_RUNNING;										//R�ckgabe, dass der Timer noch l�uft
}