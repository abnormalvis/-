/* Timer0.c */
#include <REG52.H>
#include "Timer0.h"
#include "stdint.h"

volatile uint32_t SystemTick = 0; // 系统时间基准
void Timer0Init(void)
{
    TMOD &= 0xF0;       // 清除T0配置
    TMOD |= 0x01;       // 设置T0为模式1（16位定时器）
    TH0 = 0xFC;         // 设置定时初值高字节
    TL0 = 0x18;         // 设置定时初值低字节
    TF0 = 0;            // 清除溢出标志
    TR0 = 1;            // 启动定时器0
    ET0 = 1;            // 使能T0中断
    EA = 1;             // 全局中断使能
    PT0 = 0;            // 设置低优先级
}

void Timer0_Routine(void) interrupt 1
{
    TH0 = 0xFC;         // 重装初值高字节
    TL0 = 0x18;         // 重装初值低字节
    SystemTick++;       // 系统时钟递增
}
