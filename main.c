#include <REG52.H>
#include <rtx51tny.h>
#include "Delay.h"
#include "note.h"
#include "oled.h"
#include "oledfont.h"
#include "key.h"
#include "Timer0.h"
#include "Timer1.h"
#include "music.h"
#include "systemstate.h"
#include <rtx51tny.h>
extern void os_evt_set(unsigned int event_flags, unsigned char task_id);
extern void os_evt_wait_or(unsigned int event_flags, unsigned int timeout);
//extern void os_start();
volatile unsigned char x_position = 119; // 音符初始位置
volatile unsigned char speed = 6;        // 音符移动速度

void task_init(void) _task_ 0
{
    OLED_ShowString(0, 0, "Init OK!", 16);
    OLED_Refresh(); // 必须调用刷新函数
    Delay(1000);
    P2 = 0xFF;
    OLED_Init();
    OLED_Clear();
    os_wait(K_IVL, 100, 0);

    os_create_task(1); // 键盘扫描任务
    os_create_task(2); // 状态机任务
    //os_create_task(3); // 显示任务
    os_delete_task(0); // 删除初始化任务
}

void task_key(void) _task_ 1
{
    while (1)
    {
        system_event = get_system_event();

        if (system_event != EVENT_NONE)
        {
            os_evt_set(0x0001, 2); // 通知状态机任务
            os_evt_set(0x0001, 3); // 通知显示任务
        }

        os_wait(K_IVL, 10, 0);
    }
}

void task_state_machine(void) _task_ 2
{
    while (1)
    {
        OLED_Refresh();
        os_wait(K_IVL, 50, 0);
        os_evt_wait_or(0x0001, 0xFFFF); // 等待事件
        
        switch (current_state)
        {
        case STATE_MENU:
            handle_menu_state(system_event);
            break;
        case STATE_GAME:
            handle_game_state(system_event);
            break;
        case STATE_SETTING:
            handle_setting_state(system_event);
            break;
        case STATE_SCORE:
            handle_score_state(system_event);
            break;
        case STATE_PLAYING:
            handle_play_state(system_event);
            break;
        case STATE_PAUSED:
            handle_play_state(system_event);
            break;
        }
    }
}

// 任务板

// void task_display(void) _task_ 3
// {
//     while (1)
//     {
//         OLED_Refresh(); // 调用底层刷新函数
        
//         switch (current_state)
//         {
//         case STATE_MENU:
//             //draw_menu(); // 分离显示逻辑与状态处理
//             break;
//         case STATE_GAME:
//             //draw_game();
//             break;
//         case STATE_SETTING:
//             //draw_setting();
//             break;
//         case STATE_SCORE:
//             //draw_score();
//             break;
//         default:
//             OLED_Clear();
//             //OLED_ShowString(0,0,"STATE ERROR",16);
//         }
//         os_wait(K_IVL, 100, 0); // 100ms刷新周期(10Hz)
//     }
// }
