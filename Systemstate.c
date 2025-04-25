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
    case SYSTEM_GAMEPLAY:
        //BEEP_Play(SOUND_GAME_START);    // 游戏初始化提示音
        Game_Init(); // 初始化游戏状态
        break;
    case SYSTEM_PAUSE:
        //BEEP_Play(SOUND_PAUSE);
        OLED_ShowString(0, 0, "Paused"); // 显示暂停界面
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

// void MenuHandler(void)
// {
//     // 菜单状态处理
//     Menu_Update();  // 更新菜单状态
//     Menu_Display(); // 显示菜单
// }

// void GameplayHandler(void)
// {
//     // 游戏进行状态处理
//     Game_Update();           // 更新游戏状态
//     Game_HandleInput(Key()); // 处理游戏输入
//     // 游戏画面更新
//     // Game_Display();
// }



// void PauseHandler(void)
// {
//     // 暂停状态处理
//     static uint8_t lastKey = 0;
//     uint8_t key = Key();

//     if (key != lastKey)
//     {
//         if (key == KEY_PAUSE)
//         {
//             SystemState_Transition(SYSTEM_GAMEPLAY); // 恢复游戏
//         }
//         lastKey = key;
//     }
// }

void ScoreReviewHandler(void)
{
    static uint8_t lastKey;
    uint8_t key;
    // 结算界面状态处理
    // 显示分数、评价等信息
    OLED_ShowString(0, 0, "Score: ");
    OLED_ShowChar(40, 0, ctx.score);

    // 等待用户确认，然后返回菜单
    lastKey = 0;
    key = Key();

    if (key != lastKey)
    {
        if (key == KEY_EXIT)
        {
            SystemState_Transition(SYSTEM_MENU); // 返回菜单
        }
        lastKey = key;
    }
}

// void ErrorHandler(void)
// {
//     // 异常状态处理
//     OLED_ShowString(0, 0, "System Error!");
//     Delay(2000);                            // 显示错误信息2秒
//     SystemState_Transition(SYSTEM_BOOTING); // 重新启动系统
// }

// void MusicPlayingHandler(void)
// {
//     // 音乐播放状态处理
//     PlayMusic(); // 调用音乐播放函数
// }


// void MusicLoadingHandler(void) 
// {
//     // 音乐加载状态处理
//     LoadMusicSegment(current_addr); // 加载音乐片段
//     SystemState_Transition(SYSTEM_MUSIC_PLAYING); // 加载完成后进入播放状态
// }


