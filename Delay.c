#include "Delay.h"
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