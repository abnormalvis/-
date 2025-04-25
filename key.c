#include "key.h"
#include "Delay.h"

// 按键状态
static uint8_t key_state = 0;
static uint8_t last_key = 0;
uint8_t KeyValue = 0;
uint8_t Key(void)
{
    return Get_Key(); // 根据实际需求选择
}

uint8_t Key_Scan(void)
{
    // 独立按键扫描
    if (!KEY1) return KEY_TRACK1;
    if (!KEY2) return KEY_TRACK2; 
    if (!KEY3) return KEY_TRACK3;
    if (!KEY4) return KEY_TRACK4;
    if (!KEY5) return KEY_STOP;     // 暂停键
    if (!KEY6) return KEY_EXIT;     // 退出键 
    return 0;
}

uint8_t Get_Key(void)
{
    return Key_Scan();
}

uint8_t Get_GameKey(void)
{
    uint8_t key = KeyToTrack(Get_Key()); // 转换为游戏按键
    return (key == KEY_STOP) ? 0 : key; // 过滤暂停键
}

uint8_t KeyToTrack(uint8_t key)
{
    switch (key)
    {
    case KEY_TRACK1:
        return 1;
    case KEY_TRACK2:
        return 2;
    case KEY_TRACK3:
        return 3;
    case KEY_TRACK4:
        return 4;
    default:
        return 0; // 无效按键
    }
}