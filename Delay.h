#ifndef __DELAY_H__
#define __DELAY_H__
// @11.0592MHz晶振  1ms粗略延时值
#define DELAY_1MS_COUNT 110
#include "stdint.h"
void Delay(unsigned int xms);
// void Delay_50ms(uint16_t Del_50ms);
// void Delay_1ms(uint16_t Del_1ms);
#endif
