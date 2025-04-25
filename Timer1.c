#include <REG52.H>  // 确保包含SFR定义
#include "Timer1.h"
#include "Beep.h"
#include "Musictable.h"

#ifndef _TIMER1_H_
#define _TIMER1_H_
sbit Buzzer = P2^5;  // 使用标准位寻址符号
#endif


// 或直接在Timer1.c中全局定义：
// sbit Buzzer = P2^5;  // 删除^后的空格
// void Timer1_Init(void)
// {
//     TMOD &= 0x0F;   // 清除T1配置
//     TMOD |= 0x10;   // 设置T1为模式1（16位定时器）
//     ET1 = 1;        // 使能T1中断
//     PT1 = 1;        // 设置高优先级
//     EA = 1;         // 全局中断使能
// }

void Timer1_Routine(void) interrupt 3
{
    TL1 = FreqTable[FreqSelect] & 0xFF; // 重装频率低字节
    TH1 = FreqTable[FreqSelect] >> 8;  // 重装频率高字节
    Buzzer = !Buzzer;                  // 翻转蜂鸣器状态
}

void Timer1Init(void)
{
    TMOD &= 0x0F;       // 清除T1配置
    TMOD |= 0x10;       // 设置T1为模式1（16位定时器）
    TH1 = 0xFF;         // 初始值（根据频率动态调整）
    TL1 = 0xFF;         // 初始值（根据频率动态调整）
    TR1 = 0;            // 暂停定时器1
    ET1 = 1;            // 使能T1中断
}
// void Timer1_Routine(void) interrupt 3
// {
//     TH1 = reload >> 8;  // 重装初值高字节
//     TL1 = reload;       // 重装初值低字节
//     BUZZER = ~BUZZER;   // 切换蜂鸣器状态
// }
