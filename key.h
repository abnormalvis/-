#ifndef __KEY_H
#define __KEY_H
#include <reg52.h>
#include "stdint.h"
sbit KEY1 = P1 ^ 0;
sbit KEY2 = P1 ^ 1;
sbit KEY3 = P1 ^ 2;
sbit KEY4 = P1 ^ 3;
sbit KEY5 = P1 ^ 4; // 暂停键
sbit KEY6 = P1 ^ 5; // 退出按键
#define KEY_TRACK1 1
#define KEY_TRACK2 2
#define KEY_TRACK3 3
#define KEY_TRACK4 4
#define KEY_STOP 5
#define KEY_EXIT 6
#define KEY_CONFIRM 3

extern volatile bit KeyLock; // 按键锁标志
extern uint8_t KeyValue; // 用来存放读取到的键值

uint8_t Key_Scan(void);
uint8_t Get_Key(void);
uint8_t Key(void);
uint8_t Get_GameKey(void);
uint8_t KeyToTrack(uint8_t key);
#endif