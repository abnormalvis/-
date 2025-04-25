#ifndef __SYSTEMSTATE_H_
#define __SYSTEMSTATE_H_
#include "stdint.h"
#include "Beep.h"
#include "Timer0.h"
uint32_t GetSystemTick(void);
extern volatile uint8_t signal_flags; // 全局信号标志位
typedef enum
{
    SYSTEM_BOOTING,       // 系统启动中
    SYSTEM_MENU,          // 主菜单界面
    SYSTEM_GAMEPLAY,      // 游戏进行中
    SYSTEM_IDLE,          // 空闲状态
    SYSTEM_PAUSE,         // 游戏暂停
    SYSTEM_SCORE_REVIEW,  // 结算界面
    SYSTEM_ERROR,         // 异常状态
    SYSTEM_MUSIC_PLAYING, // 音乐播放中
    SYSTEM_MUSIC_LOADING  // 音乐加载中
} SystemState;

// 事件类型（统一事件总线）
typedef enum
{
    EVENT_KEY_INPUT,     // 按键事件
    EVENT_STORAGE_READY, // 存储就绪
    EVENT_AUDIO_END,     // 音频播放完成
    EVENT_JUDGE_RESULT   // 判定结果
} EventType;

// 全局状态管理器
typedef struct
{
    volatile SystemState current_state;
    uint32_t state_enter_time; // 进入当前状态的时间戳
    uint8_t pending_events;    // 待处理事件计数
} StateManager;
void SystemState_Transition(SystemState new_state);
extern volatile StateManager g_state; // 状态管理器
#endif