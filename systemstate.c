#include "systemstate.h"
#include "oled.h"
#include "key.h"
// 全局状态变量
SystemState current_state = STATE_MENU;
SystemEvent system_event = EVENT_NONE;
u8 menu_selection = 1; // 菜单选项
u8 song_selection = 1;
//unsigned char code current_track;
void Music_Init(unsigned char code *musicData);
volatile bit is_playing = 0;
// u8 x_position = 0, hit_time = 0;
static char speed_str[3] = "0";

void handle_menu_state(SystemEvent event)
{
    OLED_Clear();

    OLED_ShowString(25, 0, "START GAME", 16);
    OLED_ShowString(25, 3, "SETTING", 16);
    OLED_ShowString(25, 6, "SCORE", 16);

    switch (event)
    {
    case EVENT_UP: // 上键处理
        menu_selection = (menu_selection == 1) ? 3 : menu_selection - 1;
        break;
    case EVENT_DOWN: // 下键
        menu_selection = (menu_selection % 3) + 1;
        break;
    case EVENT_CONFIRM: // 确认键
        switch (menu_selection)
        {
        case 1:
            current_state = STATE_SONG_SELECT;
            break;
        case 2:
            current_state = STATE_SETTING;
            break;
        case 3:
            current_state = STATE_SCORE;
            break;
        }
        break;
    }

    switch (menu_selection)
    {
    case 1:
        OLED_ShowString(3, 0, ">", 16);
        break;
    case 2:
        OLED_ShowString(3, 3, ">", 16);
        break;
    case 3:
        OLED_ShowString(3, 6, ">", 16);
        break;
    }
}

// 音乐播放
void handle_play_state(SystemEvent event)
{
    static u8 progress = 0;

    if (event == EVENT_PAUSE_RESUME)
    {   // 翻转播放状态
        is_playing = !is_playing;
        current_state = is_playing ? STATE_PLAYING : STATE_PAUSED;
        TR1 = is_playing; // 控制定时器启停
    }

    if (event == EVENT_BACK)
    {
        Music_Stop();
        current_state = STATE_SONG_SELECT;
    }

    // 更新播放显示
    OLED_Clear();
    OLED_ShowString(0, 0, "PLAYING:", 16);
    OLED_ShowString(80, 0, song_selection == 1 ? "SONG1" : "SONG2", 16);
    OLED_ShowString(0, 6, is_playing ? "[PAUSE]" : "[RESUME]", 16);

    // 播放逻辑
    if (is_playing && current_state == STATE_PLAYING)
    {
        Music_Play();
        // 简单进度显示
        if (++progress > 100)
            progress = 0;
        OLED_ShowNum(0, 3, progress, 3, 16);
    }
}

// 游戏播放
void handle_game_state(SystemEvent event)
{
    static u8 note_position = 119;

    if (event == EVENT_BACK)
    {
        current_state = STATE_MENU;
        return;
    }

    // 游戏逻辑
    OLED_ShowString(note_position, 0, ":)", 16);
    if (note_position > speed)
    {
        note_position -= speed;
    }
    else
    {
        note_position = 121 - speed * 2;
    }
}

void handle_setting_state(SystemEvent event)
{
    static u8 setting_phase = 0;
    u8 x_position, hit_time;

    x_position = 0, hit_time = 0;
    switch (event)
    { // 修改 event.type 为 event
    case EVENT_ENTER_STATE:
        OLED_Init();
        OLED_Clear();
        P2 = 0x03;
        x_position = 121 - speed * 2;
        setting_phase = 1;
        break;

    case EVENT_TIMER_TICK:
        if (setting_phase == 1)
        {
            // 更新速度显示
            speed_str[0] = (speed / 10) ? (speed / 10 + '0') : ' ';
            speed_str[1] = speed % 10 + '0';
            OLED_ShowString(3, 3, "speed:", 16);
            OLED_ShowString(60, 3, speed_str, 16);
            OLED_ShowString(16, 0, "I", 16);
            // 处理命中判定
            if (x_position < 31 && event == KEY4_PRESSED)
            {
                const char *result;
                hit_time = (x_position - 16) / speed;

                if (hit_time < 1)
                    result = "Pure";
                else if (hit_time < 2)
                    result = "far   ";
                else if (hit_time < 3)
                    result = "bad    ";
                else
                    result = "lost   ";

                OLED_ShowString(3, 6, result, 16);
                OLED_ShowString(x_position, 0, " ", 16);
                x_position = 121 - speed * 2;
            }
            if (x_position > speed && x_position < 120)
            {
                OLED_ShowString(x_position, 0, " ", 16);
                x_position -= speed;
                OLED_ShowString(x_position, 0, ":)", 16);
            }
            else
            {
                OLED_ShowString(x_position, 0, " ", 16);
                if (x_position < speed)
                    OLED_ShowString(3, 6, "lost   ", 16);
                x_position = 121 - speed * 2;
            }
        }
        break;

    case EVENT_EXIT_STATE: // 退出设置状态事件
        OLED_Clear();
        setting_phase = 0;
        break;

    default:
        break;
    }
}

void handle_song_select_state(SystemEvent event)
{
    OLED_Clear();
    OLED_ShowString(25, 0, "SONG1", 16);
    OLED_ShowString(25, 3, "SONG2", 16);
    OLED_ShowString(25, 6, "EXIT", 16);

    switch (event)
    {
    case EVENT_UP:
        song_selection = (song_selection == 1) ? 3 : song_selection - 1;
        break;
    case EVENT_DOWN:
        song_selection = (song_selection % 3) + 1;
        break;
    case EVENT_CONFIRM:
        switch (song_selection)
        {
        case 1:
            //current_track =  Flowertower;
            Music_Init(Flowertower);
            current_state = STATE_PLAYING;
            is_playing = 1;
            break;
        case 2:
            //current_track = Emptybox;
            Music_Init(Emptybox);
            current_state = STATE_PLAYING;
            is_playing = 1;
            break;
        case 3:
            current_state = STATE_MENU;
            break;
        }
        break;
    case EVENT_BACK:
        current_state = STATE_MENU;
        break;
    }

    // 显示选中箭头
    switch (song_selection)
    {
    case 1:
        OLED_ShowString(3, 0, ">", 16);
        break;
    case 2:
        OLED_ShowString(3, 3, ">", 16);
        break;
    case 3:
        OLED_ShowString(3, 6, ">", 16);
        break;
    }
}
void handle_score_state(SystemEvent event)
{
    static u8 score_phase = 0;

    switch (event)
    {
    case EVENT_ENTER_STATE:
        OLED_Clear();
        score_phase = 1;
        break;

    case EVENT_TIMER_TICK:
        if (score_phase == 1)
        {
            OLED_ShowString(3, 0, "score:", 16);
        }
        break;

    case EVENT_EXIT_STATE:
        score_phase = 0;
        break;

    default:
        break;
    }
}

SystemEvent get_system_event()
{
    static bit key1_last = 1, key2_last = 1, key3_last = 1, key4_last = 1;

    if (!KEY1 && key1_last)
    {
        key1_last = 0;
        return EVENT_UP;
    }
    if (!KEY2 && key2_last)
    {
        key2_last = 0;
        return EVENT_DOWN;
    }
    if (!KEY3 && key3_last)
    {
        key3_last = 0;
        return EVENT_CONFIRM;
    }

    if(!KEY4 && key4_last)
    {
        key4_last = 0;
        return EVENT_PAUSE_RESUME;
    }
    if (KEY1)
        key1_last = 1;
    if (KEY2)
        key2_last = 1;
    if (KEY3)
        key3_last = 1;
    if(KEY4)
        key4_last = 1;
    return EVENT_NONE;
}