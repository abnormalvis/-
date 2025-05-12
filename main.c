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

// �������ȼ�����
#define TASK_OLED 1
#define TASK_GAME 2
#define TASK_INPUT 3
#define TASK_AUDIO 4

// ȫ���źŶ���
#define SIG_INPUT 0x01             // 0x01
#define SIG_GAME_UPDATE 0x02       // 0x02
#define SIG_OLED_REFRESH 0x04      // 0x04

/*
����һ�����ڴ����û����ڲ˵�ҳ��ʱ�İ�������
*/
void Menu_HandleInput(unsigned char key)
{
    switch (menu.current_page)
    {
    case MENU_MAIN:            // ���˵�
        if (key == KEY_TRACK1) // ����ѡ����
        {
            if (menu.selected_item > 0)
                menu.selected_item--;
        }
        else if (key == KEY_TRACK2) // ����ѡ����
        {
            if (menu.selected_item < menu.max_items - 1)
                menu.selected_item++;
        }
        else if (key == KEY_CONFIRM) // ȷ��ѡ����
        {
            switch (menu.selected_item)
            {
            case 0: // ������Ϸ
                Menu_TransitionTo(MENU_PLAY);
                break;
            case 1: // ����
                Menu_TransitionTo(MENU_SETTINGS);
                break;
            case 2: // ����
                OLED_Clear();
                OLED_ShowString(0, 0, "Help: Use keys", 8);
                OLED_ShowString(0, 2, "to navigate and play!.", 8);
                OLED_Refresh();
                Delay(2000); // ��ʾ������Ϣ2��
                break;
            case 3: // �˳�
                OLED_Clear();
                OLED_ShowString(0, 0, "Exiting...", 8);
                OLED_Refresh();
                Delay(1000);
                SystemState_Transition(SYSTEM_MENU);
                break;
            }
        }
        break;

    case MENU_SETTINGS:                             // ���ò˵�
        if (key == KEY_TRACK1 || key == KEY_TRACK2) // �л�����
        {
            menu.language = (menu.language == 1) ? 2 : 1; // 1: EN  2: CN
        }
        else if (key == KEY_CONFIRM) // �������˵�
        {
            Menu_TransitionTo(MENU_MAIN);
        }
        break;

    case MENU_PLAY:            // ��Ϸ�˵�
        if (key == KEY_TRACK1) // ����ѡ����
        {
            if (menu.selected_item > 0)
                menu.selected_item--;
        }
        else if (key == KEY_TRACK2) // ����ѡ����
        {
            if (menu.selected_item < menu.max_items - 1)
                menu.selected_item++;
        }
        else if (key == KEY_CONFIRM) // ȷ��ѡ����
        {
            if (menu.selected_item < MAX_SONG_NUM) // ѡ�����
            {
                menu.song_selection = menu.selected_item;
                Game_Start(menu.song_selection); // ��ʼ��Ϸ
                PlayMusic();                     // ��������
                SystemState_Transition(SYSTEM_GAMEPLAY);
            }
            else // �������˵�
            {
                Menu_TransitionTo(MENU_MAIN);
            }
        }
        break;

    default:
        break;
    }

    // ˢ�²˵���ʾ
    Menu_Display();
}
// ϵͳ��ʼ��
void System_Init(void)
{
    FreqSelect = 0;
    EA = 0;                           // �����ж�
    Timer0Init();                     // ��ʼ����ʱ��0��ϵͳʱ�ӣ�
    Timer1Init();                     // ��ʼ����ʱ��1�����ֲ��ţ�
    OLED_Init();                      // ��ʼ��OLED��ʾ��
    //AT24C1024_Write(0x0000, NULL, 0); // ��ʼ���洢
    Menu_Init();                      // ��ʼ���˵�
    Game_Init();                      // ��ʼ����Ϸ
    EA = 1;                           // �����ж�
}

// OLED ˢ������
void Task_OLED(void) _task_ TASK_OLED
{
    while (1)
    {
        os_wait1(0); // �ȴ������ź�
        if (signal_flags & SIG_OLED_REFRESH)
        {                                      // ������źű�־��ˢ�� OLED ��ʾ������
            signal_flags &= ~SIG_OLED_REFRESH; // ����źű�־  ��signal_flags��oledˢ���ź�ȡ���Ľ���������� ���źű�־λ����
            
            Menu_Display();                    // ˢ�²˵���ʾ
            OLED_Refresh();                    // ˢ�� OLED ��ʾ������
        }
    }
}

// ������������  ���ڼ���û��İ���������Ϣ
void Task_Input(void) _task_ TASK_INPUT
{
    uint8_t key; // ��ȡ��ǰ����ֵ
    while (1)
    {
        os_wait1(0); // �ȴ������ź�
        if (signal_flags & SIG_INPUT)
        {                               // �źű�־λ��0x01���������㣬������Ϊ�棬˵���а�������
            signal_flags &= ~SIG_INPUT; // ����źű�־
            key = Get_Key();            // ��ȡ����ֵ

            // ���ݵ�ǰϵͳ״̬����������
            switch (g_state.current_state)
            {
            case SYSTEM_MENU:
                // �����ǰ״̬���ڲ˵�״̬������˵�����
                Menu_HandleInput(key);
                break;
            case SYSTEM_GAMEPLAY:
                // �����ǰ������Ϸ���棬������Ϸ����
                Game_HandleInput(key);
                break;
            case SYSTEM_SCORE_REVIEW:
                if (key == KEY_CONFIRM)
                {
                    // �����ǰ�����������״̬ Ӧ�ý���������չʾ���棬���ҵȴ����ȷ���ٷ������˵�
                    Menu_TransitionTo(MENU_MAIN);        // �ȸ��²˵�״̬
                    SystemState_Transition(SYSTEM_MENU); // �ڸ���ϵͳ״̬
                }
                break;
            default:
                break;
            }
        }
    }
}

// ���ֲ�������
void Task_Audio(void) _task_ TASK_AUDIO
{
    while (1)
    {
        PlayMusic(); // ��������
        os_wait1(0); // ��ͣ���񣬵ȴ�ϵͳ״̬����
    }
}

void Task_Game(void) _task_ TASK_GAME
{
    while (1)
    {
        // ����ϵͳ״̬�����źű�־
        switch (g_state.current_state)
        {
        case SYSTEM_MENU:
            // �����ǰ״̬���ڲ˵�״̬����ˢ��OLED��ʾ�Ļ�����
            signal_flags |= SIG_OLED_REFRESH; // ֻҪoled��ˢ���źţ����ߵ�ǰΪ�˵�״̬����ˢ��oled
            isr_send_signal(TASK_OLED);       // ����oled�����ź�
            break;
        case SYSTEM_GAMEPLAY:
            // �����ǰ״̬������Ϸ״̬��������Ϸ�����ź�
            signal_flags |= SIG_GAME_UPDATE; // ֻҪ��Ϸ�и����źţ��͸�����Ϸ
            isr_send_signal(TASK_GAME);      // ������Ϸ�����ź�
            break;
        case SYSTEM_SCORE_REVIEW:
            // �����ǰ���ڳɼ��ع�״̬������oledˢ���ź�
            signal_flags |= SIG_OLED_REFRESH; // ֻҪ�гɼ����£���ˢ��oled
            isr_send_signal(TASK_OLED);
            break;
        default:
            // ������������������״̬�е��κ�һ������ʲôҲ���� ֱ������
            break;
        }

        // ���������ź�
        if (Key_Scan())
        {
            signal_flags |= SIG_INPUT;   // ���ð��������ź�    �����ǰSIG_INPUT�ź�Ϊ1���򲻷����ź� �������ź�
            isr_send_signal(TASK_INPUT); // ���������ź�
        }

        Delay(10); // ��ѭ����ʱ��������ߵ� CPU ռ��
    }
}

// ������
void init_task(void) _task_ 0
{
    System_Init(); // ϵͳ��ʼ��

    // ��������
    os_create_task(TASK_OLED);  // OLED ˢ������
    os_create_task(TASK_GAME);  // ��Ϸ��������
    os_create_task(TASK_INPUT); // ������������
    os_create_task(TASK_AUDIO); // ���ֲ�������

    // ����ϵͳ״̬��
    SystemState_Transition(SYSTEM_BOOTING);

    os_delete_task(0); // ɾ����ʼ������
}