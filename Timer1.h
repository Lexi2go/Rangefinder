/*
 * Timer1.h
 *
 * Created: 02.12.2022 08:23:21
 *  Author: Students
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define TIMER_TRIGGERED 1
#define TIMER_RUNNING 0

extern unsigned char ucFlag10ms, ucFlag_Tim;
extern unsigned short uszaehler;


void Timer1_Init(void);
void EI_Init();
unsigned short Umrechnung();
//unsigned long Timermessung_anhalten();
//unsigned char Timer1_get_10usState(void);

#endif /* TIMER1_H_ */