#ifndef __KEY_H
#define __KEY_H
#include <reg52.h>
#include "stdint.h"
// P3.0 (KEY_PLAY)  -> 游玩键（绿色按钮）
// P3.1 (KEY_PAUSE) -> 暂停键（红色按钮）
// P3.2 (KEY_DOWN)  -> 菜单下键
#define KEY_TRACK1 1
#define KEY1 P3^0
#define KEY2 P3^1
#define KEY3 P3^2
#define KEY4 P3^3
// #define KEY_TRACK2 2
// #define KEY_TRACK3 3
// #define KEY_TRACK4 4
#define KEY_STOP 5
#define KEY_PREV    1  // KEY1 上/暂停
#define KEY_CONFIRM 2  // KEY2 确认/游玩
#define KEY_NEXT    3  // KEY3 下

extern uint8_t KeyValue; // 用来存放读取到的键值

uint8_t Key_Scan(void);
uint8_t Get_Key(void);
uint8_t Key(void);
uint8_t Get_GameKey(void);
uint8_t KeyToTrack(uint8_t key);
#endif