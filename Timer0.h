#ifndef __TIMER0_H__
#define __TIMER0_H__
#include "Delay.h"
#include "note.h"
#include "oled.h"
#include "oledfont.h"
#include "key.h"
#include "Timer0.h"
#include "Timer1.h"
#include "music.h"
extern volatile u32 SystemTick; // 系统时间基准（毫秒）
//extern unsigned char FreqSelect;
void Timer0Init(void);

#endif
