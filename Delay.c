#include "Delay.h"
#include "stdint.h"

void Delay(unsigned int xms)
{
    unsigned char i, j;
    while (xms--)
    {
        i = 2;
        j = 229; // 调整 j 的值以适应 11.0592MHz 晶振频率
        do
        {
            while (--j)
                ;
        } while (--i);
    }
}

// void Delay_50ms(uint16_t Del_50ms) // 修正参数类型
// {
//     while (Del_50ms--)
//     {
//         uint16_t i = 55000; // 近似 50ms 延时
//         while (i--)
//             ;
//     }
// }

// void Delay_1ms(uint16_t Del_1ms) 
// {
//     while (Del_1ms--)
//     {
//         uint16_t i = 1000; // 近似 1ms 延时
//         while (i--)
//             ;
//     }
// }