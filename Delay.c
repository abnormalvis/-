#include "Delay.h"
#include "stdint.h"

void Delay(unsigned int xms)
{
    unsigned char i, j;
    while (xms--)
    {
        i = 2;
        j = 229; // ���� j ��ֵ����Ӧ 11.0592MHz ����Ƶ��
        do
        {
            while (--j)
                ;
        } while (--i);
    }
}

// void Delay_50ms(uint16_t Del_50ms) // ������������
// {
//     while (Del_50ms--)
//     {
//         uint16_t i = 55000; // ���� 50ms ��ʱ
//         while (i--)
//             ;
//     }
// }

// void Delay_1ms(uint16_t Del_1ms) 
// {
//     while (Del_1ms--)
//     {
//         uint16_t i = 1000; // ���� 1ms ��ʱ
//         while (i--)
//             ;
//     }
// }