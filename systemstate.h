#ifndef _SYSTEMSTATE_H_
#define _SYSTEMSTATE_H_
#define XDATA __xdata
#define CODE __code
#include "key.h"
#include "Delay.h"
#include "note.h"
#include "oled.h"
#include "oledfont.h"
#include "key.h"
#include "Timer0.h"
#include "Timer1.h"
#include "music.h"

// extern u8 x_position;
// extern u8 hit_time;
typedef enum
{
    STATE_MENU,
    STATE_GAME,
    STATE_SETTING,
    STATE_SCORE,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_SONG_SELECT
} SystemState;

typedef enum
{
    EVENT_NONE,
    EVENT_UP,
    EVENT_DOWN,
    EVENT_CONFIRM,
    EVENT_BACK,
    EVENT_ENTER_STATE, // 添加暂停或继续类型
    EVENT_TIMER_TICK,
    EVENT_EXIT_STATE,
    EVENT_PAUSE_RESUME

} SystemEvent;
void change_state(SystemState new_state);
void enqueue_event(SystemEvent evt);
void handle_exit_state(SystemState state);
void handle_enter_state(SystemState state);
SystemEvent get_system_event(void);
void handle_menu_state(SystemEvent event);
void handle_game_state(SystemEvent event);
void handle_setting_state(SystemEvent event);
void handle_score_state(SystemEvent event);
void handle_play_state(SystemEvent event);
extern SystemState current_state;
extern SystemEvent system_event;
#endif