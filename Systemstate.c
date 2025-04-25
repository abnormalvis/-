#include "SystemState.h"
#include "key.h"
#include "Game.h"
#include "Beep.h"
#include "oled12864.h"
#include "AT24C1024B.h"
#include "Musictable.h"
#include "menu.h"
#include "Delay.h"
#include "Beep.h"
#include "Timer1.h"
#include "Timer0.h"
#include "stdint.h"
#include <Reg52.h>

volatile uint8_t signal_flags = 0; // 初始化为0
// 系统状态转换
void SystemState_Transition(SystemState new_state)
{
    uint8_t Key;
    EA = 0; // 关中断保证原子操作
    g_state.current_state = new_state;
    g_state.state_enter_time = GetSystemTick(); // 直接使用定时器0的计数
    EA = 1;
    Key = Get_Key(); // 获取按键值
    // 状态进入处理
    switch (new_state)
    {
    case SYSTEM_MENU:
        Menu_Update();
        break;
    case SYSTEM_GAMEPLAY:
        // BEEP_Play(SOUND_GAME_START);    // 游戏初始化提示音
        Game_Init(); // 初始化游戏状态
        break;
        if (new_state == SYSTEM_PAUSE)
        {

            OLED_Clear();
            OLED_ShowString(40, 3, "[PAUSED]", 16);
            OLED_ShowString(32, 5, "PRESS KEY1 TO RESUME", 8);
            OLED_Refresh();
        }
        break;
    case EVENT_KEY_INPUT:
        if (Key == 6)
        {
            Game_Init(); // 重置游戏
            g_state.current_state = SYSTEM_MENU;
        }
        break;
    }
    EA = 1; // 恢复中断
}

uint32_t GetSystemTick(void)
{
    return SystemTick; // 返回系统时间戳
}

volatile StateManager g_state =
    {
        SYSTEM_BOOTING, /* 初始状态 */
        0,              /* 进入当前状态的时间戳 */
        0               /* 待处理事件计数 */
};

// 启动状态句柄
void BootingHandler(void)
{
    // 系统启动处理
    OLED_Init();                         // 初始化OLED显示屏
    Delay(500);                          // 短暂延迟，模拟启动过程
    SystemState_Transition(SYSTEM_MENU); // 启动完成后进入菜单状态
}

