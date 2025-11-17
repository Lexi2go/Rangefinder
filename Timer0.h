/*
 * Timer0.h
 *
 * Created: 29.04.2022 10:43:35
 *  Author: Students
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define TIMER_TRIGGERED 1
#define TIMER_RUNNING 0

extern unsigned char ucFlag10ms;

void Timer0_Init(void);
unsigned char Timer0_get_10usState(void);
unsigned char Timer0_get_10msState(void);
unsigned char Timer0_get_20msState(void);
unsigned char Timer0_get_1sState(void);





#endif /* TIMER0_H_ */