#ifndef __TIMER1_H__
#define __TIMER1_H__
#include <REG52.H>
sbit Buzzer = P2^5; 
void Timer1_Init(void);
void Timer1_SetFreq(unsigned int freq);

#endif