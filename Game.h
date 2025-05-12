#ifndef __GAME_H
#define __GAME_H

#include "key.h"
#include "oled12864.h"
#include "Systemstate.h"
#include "Beep.h"
#include "Musictable.h"
#include "oledfont.h"
#include "Timer1.h"
#include "Timer0.h"
#include "Delay.h"
#include "menu.h"
#include "stdint.h"
#include "stdbool.h"

// 硬件相关常量
#define JUDGE_LINE_X 31     // 判定线X坐标
#define COMBO_RECOVERY 20   // 每20连击恢复生命
#define INPUT_COOLDOWN 50   // 输入冷却时间(ms)
#define JUDGE_BAD_RANGE 150 // BAD判定时间范围(ms)
// 在Game.h中添加以下定义
#define JUDGE_ADVANCE 100           // 音符提前生成时间
extern volatile bool score_updated; // 分数更新标志
#define MAX_NOTES 10                // 最大音符数量
#define NOTE_WIDTH 8    // 音符位图宽度
#define NOTE_HEIGHT 8   // 音符位图高度
extern uint32_t high_score;
extern uint8_t xdata current_music_buf[1024];
typedef struct
{
    uint32_t start_time; // 音符开始时间
    uint32_t end_time;   // 音符结束时间
    uint32_t start_idx;  // 音符开始索引
} TimeSlice;

extern uint32_t beat_ms(uint16_t bpm); 
// 音符类型
typedef enum
{
    NOTE_NORMAL,
    NOTE_HOLD
} NoteType;

// 事件类型
typedef enum
{
    GAME_EVENT_NOTE_HIT, // 音符被击中
    GAME_EVENT_PAUSE,    // 游戏暂停
    GAME_EVENT_RESUME    // 游戏继续
} GameEvent;

// 判定等级
typedef enum
{
    JUDGE_PERFECT = 0, // 完美
    JUDGE_GOOD,        // 好
    JUDGE_BAD,         // 差
    JUDGE_MISS,        // 未击中
    JUDGE_LEVEL_MAX = 4
} JudgeLevel;
typedef struct
{
    uint8_t track;   // 总共有1-4轨道会出现音符
    uint32_t timing; // 触发时间(ms)
    bool active;     // 是否激活
    bool processed;  // 是否已处理
    NoteType type;   // 新增音符类型字段
} Note;
// 游戏上下文
typedef struct
{
    Note *notes;                            // 当前音符池
    uint32_t note_count;                    // 当前音符数量
    uint32_t current_index;                 // 当前音符索引
    uint32_t score;                         // 当前分数
    uint32_t combo;                         // 连击数
    uint16_t hp;                            // 当前生命值
    uint32_t judge_counts[JUDGE_LEVEL_MAX]; // 判定计数器
    uint32_t start_time;                    // 游戏开始时间
    uint16_t bpm;                           // BPM
    bool is_paused;                         // 游戏是否暂停
} GameContext;

extern GameContext ctx;

static void AddNoteToPool(Note note);                        // 添加音符到池中
static void BuildTimeIndex(void);                            // 构建时间索引
static int FindStartIndex(uint32_t target_time);             // 查找起始索引
static void JudgeNote(uint8_t track, uint32_t current_time); // 音符判定
void Game_Init(void);                                        // 初始化游戏
void Game_Start(uint8_t song_id);                            // 开始游戏
void Game_Update(void);                                      // 更新游戏状态
void Game_Draw(void);                                        // 绘制游戏界面
void Game_HandleInput(uint8_t key);                          // 处理输入
void Game_SendEvent(GameEvent event, uint8_t param);         // 发送事件
uint32_t GetSongNoteCount(uint8_t song_id);                  // 获取歌曲音符数量
void SaveHighScore(uint32_t score);                          // 保存高分
void ShowResult(void);                                       // 显示结果
uint32_t LoadHighScore(void); // 新增声明
// 全局上下文
extern GameContext ctx;          // 游戏上下文
extern const Note *current_song; // 当前歌曲数据

#endif