#include <REG52.H>  
#include "Timer1.h"
#include "Timer0.h"
typedef unsigned int uint32_t;
volatile uint32_t SystemTick = 0;
#ifndef _TIMER1_H_
#define _TIMER1_H_ 
#endif
void Timer0_Init(void)
{
    TMOD &= 0xF0;       // Clear T0 config
    TMOD |= 0x01;       // Mode 1 (16-bit)
    TH0 = 0xFC;         // 1ms @11.0592MHz
    TL0 = 0x18;
    ET0 = 1;            // Enable interrupt
    TR0 = 1;            // Start timer
    EA = 1;             // Global enable
}

void Timer0_ISR(void) interrupt 1 using 1 // 使用寄存器组1
{
    TH0 = 0xFC;         // Reload values
    TL0 = 0x18;
    SystemTick++;       // Update system tick
}
