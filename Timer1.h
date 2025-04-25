/*
Timer0 分配

功能：系统基准时钟
模式：模式1（16位定时器）
中断周期：1ms
用途：
维护SystemTick系统时钟
游戏逻辑更新计时
音符生成计时
按键扫描周期控制（可选）
Timer1 分配

功能：音频生成
模式：模式1（16位定时器）
动态重载：根据音符频率动态调整
用途：
音乐播放频率控制
音效生成
*/
#ifndef __TIMER1_H__
#define __TIMER1_H__
extern unsigned char FreqSelect;
void Timer1Init(void);

#endif