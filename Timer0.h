#ifndef __TIMER0_H__
#define __TIMER0_H__
#include "stdint.h"
extern volatile uint32_t SystemTick; // 系统时间基准（毫秒）
extern unsigned char FreqSelect;
void Timer0Init(void);

#endif
