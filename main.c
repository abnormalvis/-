#include <REG52.H>
// #include <rtx51tny.h>
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
// extern void os_start();
volatile unsigned char x_position = 119; // ������ʼλ��
volatile unsigned char speed = 6;        // �����ƶ��ٶ�
// ��main.c��include�������
void System_Init(void);      // ϵͳ��ʼ������
void Key_Scan_Delay(u16 ms); 

void main()
{
    u8 i;
    OLED_Init(); 
    OLED_Clear(); 
    
    while(1)
    {
        // ����1����ʾ�ַ����ַ���
        OLED_ShowChar(0, 0, 'A', 16); // 16������
        OLED_ShowString(20, 0, "Hello OLED", 16);
        OLED_ShowString(0, 3, "Test Program", 16);
        OLED_Refresh();
        Delay(2000);
        OLED_Clear();

        // ����2����ʾ����
        OLED_ShowNum(0, 0, 123456, 6, 16);
        OLED_ShowString(0, 2, "Count:", 16);
        for(i=0; i<100; i++){
            OLED_ShowNum(60, 2, i, 3, 16);
            OLED_Refresh();
            Delay(100);
        }
        OLED_Clear();

        // ����3������ͼ��
        OLED_DrawPoint(64, 32, 1); // ���ĵ�
        OLED_Fill(10, 10, 30, 30, 1); // ������
        OLED_Refresh();
        Delay(2000);
        OLED_Clear();

        // ����4��������ʾ
        OLED_ShowString(0, 0, "Scroll Test", 16);
        OLED_Refresh();
        Delay(1000);
        OLED_roll(1, 3); // ����ˮƽ����
        Delay(3000);
        OLED_roll(0, 0); // �رչ���
        OLED_Clear();
    }
}

// void main()
// {
    // System_Init();
    // while (1)
    // {
    //     system_event = get_system_event(); // ��������ȡ�¼�
    //     switch (current_state)
    //     { // ״̬�ַ�
    //     case STATE_MENU:
    //         handle_menu_state(system_event);
    //         OLED_Refresh();
    //         break;
    //     case STATE_GAME:
    //         handle_game_state(system_event);
    //         Music_Update(); // ֱ����״̬�д�������
    //         break;
    //     case STATE_SETTING:
    //         handle_setting_state(system_event);
    //         break;
    //     case STATE_SCORE:
    //         handle_score_state(system_event);
    //         break;
    //     case STATE_PLAYING:
    //         handle_play_state(system_event);
    //         break;
    //     case STATE_PAUSED:
    //         handle_play_state(system_event);
    //         break;
    //     }
    //     Key_Scan_Delay(10); // ���ԭtask1��10msɨ��
    // }
// }

// void task_init(void) _task_ 0
// {
//     OLED_ShowString(0, 0, "Init OK!", 16);
//     OLED_Refresh(); // �������ˢ�º���
//     Delay(1000);
//     P2 = 0xFF;
//     OLED_Init();
//     OLED_Clear();
//     os_wait(K_IVL, 100, 0);

//     os_create_task(1); // ����ɨ������
//     os_create_task(2); // ״̬������
//     // os_create_task(3); // ��ʾ����
//     os_delete_task(0); // ɾ����ʼ������
// }

// void task_key(void) _task_ 1
// {
//     while (1)
//     {
//         system_event = get_system_event();

//         if (system_event != EVENT_NONE)
//         {
//             os_evt_set(0x0001, 2); // ֪ͨ״̬������
//             os_evt_set(0x0001, 3); // ֪ͨ��ʾ����
//         }

//         os_wait(K_IVL, 10, 0);
//     }
// }

// void task_state_machine(void) _task_ 2
// {
//     while (1)
//     {
//         OLED_Refresh();
//         os_wait(K_IVL, 50, 0);
//         os_evt_wait_or(0x0001, 0xFFFF); // �ȴ��¼�

//         switch (current_state)
//         {
//         case STATE_MENU:
//             handle_menu_state(system_event);
//             break;
//         case STATE_GAME:
//             handle_game_state(system_event);
//             break;
//         case STATE_SETTING:
//             handle_setting_state(system_event);
//             break;
//         case STATE_SCORE:
//             handle_score_state(system_event);
//             break;
//         case STATE_PLAYING:
//             handle_play_state(system_event);
//             break;
//         case STATE_PAUSED:
//             handle_play_state(system_event);
//             break;
//         }
//     }
// }

// �����

// void task_display(void) _task_ 3
// {
//     while (1)
//     {
//         OLED_Refresh(); // ���õײ�ˢ�º���

//         switch (current_state)
//         {
//         case STATE_MENU:
//             //draw_menu(); // ������ʾ�߼���״̬����
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
//         os_wait(K_IVL, 100, 0); // 100msˢ������(10Hz)
//     }
// }
