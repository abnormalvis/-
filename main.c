#include <RTX51TNY.h>
#include "Systemstate.h"
#include "Beep.h"
#include "Game.h"
#include "Key.h"
#include "AT24C1024B.h"
#include "oled12864.h"
#include "menu.h"
#include "Delay.h"
#include "stdint.h"
#include "Timer1.h"
#include "Timer0.h"
#include "stdbool.h"
#include "Beep.h"
#include "Musictable.h"

// 任务优先级定义
#define TASK_OLED 1
#define TASK_GAME 2
#define TASK_INPUT 3
#define TASK_AUDIO 4

// 全局信号定义
#define SIG_INPUT 0x01             // 0x01
#define SIG_GAME_UPDATE 0x02       // 0x02
#define SIG_OLED_REFRESH 0x04      // 0x04

/*
定义一个用于处理用户处于菜单页面时的按键输入
*/
void Menu_HandleInput(unsigned char key)
{
    switch (menu.current_page)
    {
    case MENU_MAIN:            // 主菜单
        if (key == KEY_TRACK1) // 上移选中项
        {
            if (menu.selected_item > 0)
                menu.selected_item--;
        }
        else if (key == KEY_TRACK2) // 下移选中项
        {
            if (menu.selected_item < menu.max_items - 1)
                menu.selected_item++;
        }
        else if (key == KEY_CONFIRM) // 确认选中项
        {
            switch (menu.selected_item)
            {
            case 0: // 进入游戏
                Menu_TransitionTo(MENU_PLAY);
                break;
            case 1: // 设置
                Menu_TransitionTo(MENU_SETTINGS);
                break;
            case 2: // 帮助
                OLED_Clear();
                OLED_ShowString(0, 0, "Help: Use keys", 8);
                OLED_ShowString(0, 2, "to navigate and play!.", 8);
                OLED_Refresh();
                Delay(2000); // 显示帮助信息2秒
                break;
            case 3: // 退出
                OLED_Clear();
                OLED_ShowString(0, 0, "Exiting...", 8);
                OLED_Refresh();
                Delay(1000);
                SystemState_Transition(SYSTEM_MENU);
                break;
            }
        }
        break;

    case MENU_SETTINGS:                             // 设置菜单
        if (key == KEY_TRACK1 || key == KEY_TRACK2) // 切换语言
        {
            menu.language = (menu.language == 1) ? 2 : 1; // 1: EN  2: CN
        }
        else if (key == KEY_CONFIRM) // 返回主菜单
        {
            Menu_TransitionTo(MENU_MAIN);
        }
        break;

    case MENU_PLAY:            // 游戏菜单
        if (key == KEY_TRACK1) // 上移选中项
        {
            if (menu.selected_item > 0)
                menu.selected_item--;
        }
        else if (key == KEY_TRACK2) // 下移选中项
        {
            if (menu.selected_item < menu.max_items - 1)
                menu.selected_item++;
        }
        else if (key == KEY_CONFIRM) // 确认选中项
        {
            if (menu.selected_item < MAX_SONG_NUM) // 选择歌曲
            {
                menu.song_selection = menu.selected_item;
                Game_Start(menu.song_selection); // 开始游戏
                PlayMusic();                     // 播放音乐
                SystemState_Transition(SYSTEM_GAMEPLAY);
            }
            else // 返回主菜单
            {
                Menu_TransitionTo(MENU_MAIN);
            }
        }
        break;

    default:
        break;
    }

    // 刷新菜单显示
    Menu_Display();
}
// 系统初始化
void System_Init(void)
{
    FreqSelect = 0;
    EA = 0;                           // 禁用中断
    Timer0Init();                     // 初始化定时器0（系统时钟）
    Timer1Init();                     // 初始化定时器1（音乐播放）
    OLED_Init();                      // 初始化OLED显示屏
    //AT24C1024_Write(0x0000, NULL, 0); // 初始化存储
    Menu_Init();                      // 初始化菜单
    Game_Init();                      // 初始化游戏
    EA = 1;                           // 启用中断
}

// OLED 刷新任务
void Task_OLED(void) _task_ TASK_OLED
{
    while (1)
    {
        os_wait1(0); // 等待任务信号
        if (signal_flags & SIG_OLED_REFRESH)
        {                                      // 如果有信号标志，刷新 OLED 显示缓冲区
            signal_flags &= ~SIG_OLED_REFRESH; // 清除信号标志  将signal_flags与oled刷新信号取反的结果作与运算 将信号标志位清零
            
            Menu_Display();                    // 刷新菜单显示
            OLED_Refresh();                    // 刷新 OLED 显示缓冲区
        }
    }
}

// 按键输入任务  用于检测用户的按键交互信息
void Task_Input(void) _task_ TASK_INPUT
{
    uint8_t key; // 获取当前按键值
    while (1)
    {
        os_wait1(0); // 等待任务信号
        if (signal_flags & SIG_INPUT)
        {                               // 信号标志位与0x01进行与运算，如果结果为真，说明有按键输入
            signal_flags &= ~SIG_INPUT; // 清除信号标志
            key = Get_Key();            // 获取按键值

            // 根据当前系统状态处理按键输入
            switch (g_state.current_state)
            {
            case SYSTEM_MENU:
                // 如果当前状态处于菜单状态，则处理菜单输入
                Menu_HandleInput(key);
                break;
            case SYSTEM_GAMEPLAY:
                // 如果当前处于游戏界面，则处理游戏输入
                Game_HandleInput(key);
                break;
            case SYSTEM_SCORE_REVIEW:
                if (key == KEY_CONFIRM)
                {
                    // 如果当前处于游玩结束状态 应该进入游玩结果展示界面，并且等待玩家确认再返回主菜单
                    Menu_TransitionTo(MENU_MAIN);        // 先更新菜单状态
                    SystemState_Transition(SYSTEM_MENU); // 在更新系统状态
                }
                break;
            default:
                break;
            }
        }
    }
}

// 音乐播放任务
void Task_Audio(void) _task_ TASK_AUDIO
{
    while (1)
    {
        PlayMusic(); // 播放音乐
        os_wait1(0); // 暂停任务，等待系统状态更新
    }
}

void Task_Game(void) _task_ TASK_GAME
{
    while (1)
    {
        // 根据系统状态设置信号标志
        switch (g_state.current_state)
        {
        case SYSTEM_MENU:
            // 如果当前状态处于菜单状态，则刷新OLED显示的缓冲区
            signal_flags |= SIG_OLED_REFRESH; // 只要oled有刷新信号，或者当前为菜单状态，就刷新oled
            isr_send_signal(TASK_OLED);       // 发送oled任务信号
            break;
        case SYSTEM_GAMEPLAY:
            // 如果当前状态处于游戏状态，则发送游戏更新信号
            signal_flags |= SIG_GAME_UPDATE; // 只要游戏有更新信号，就更新游戏
            isr_send_signal(TASK_GAME);      // 发送游戏更新信号
            break;
        case SYSTEM_SCORE_REVIEW:
            // 如果当前处于成绩回顾状态，则发送oled刷新信号
            signal_flags |= SIG_OLED_REFRESH; // 只要有成绩更新，就刷新oled
            isr_send_signal(TASK_OLED);
            break;
        default:
            // 如果不处于上面的三种状态中的任何一个，则什么也不做 直接跳过
            break;
        }

        // 按键输入信号
        if (Key_Scan())
        {
            signal_flags |= SIG_INPUT;   // 设置按键输入信号    如果当前SIG_INPUT信号为1，则不发送信号 否则发送信号
            isr_send_signal(TASK_INPUT); // 发送任务信号
        }

        Delay(10); // 主循环延时，避免过高的 CPU 占用
    }
}

// 主函数
void init_task(void) _task_ 0
{
    System_Init(); // 系统初始化

    // 创建任务
    os_create_task(TASK_OLED);  // OLED 刷新任务
    os_create_task(TASK_GAME);  // 游戏更新任务
    os_create_task(TASK_INPUT); // 按键输入任务
    os_create_task(TASK_AUDIO); // 音乐播放任务

    // 启动系统状态机
    SystemState_Transition(SYSTEM_BOOTING);

    os_delete_task(0); // 删除初始化任务
}