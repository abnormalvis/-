#ifndef KEY_H
#define KEY_H
#include <REG52.H>
sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;
#define KEY4_PRESSED 4
unsigned char Key();

#endif 
