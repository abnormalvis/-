#ifndef __BEEP_H__
#define __BEEP_H__
#include <REG52.H>
sbit BUZZER = P2^5;
#include "Delay.h"
#include "Timer1.h"
#include "Musictable.h"
#define REST_NOTE 0xFF
#define MAX_SONGS 3
#define SONG_NAME_MAX_LEN 20
#ifndef KEY_PAUSE
#define KEY_PAUSE 2 // 音乐播放暂停键
#endif
#define BEEP_ON() (Buzzer = 1)  // 蜂鸣器开
#define BEEP_OFF() (Buzzer = 0) // 蜂鸣器关
extern uint32_t current_addr;        // 当前播放地址
extern volatile uint8_t current_buf; // 当前播放缓冲区
typedef enum
{                    // 声音类型结构体
    SOUND_PAUSE,     // 暂停播放
    SOUND_GAME_OVER, // 游戏结束
    SOUND_MENU,       // 菜单
    SOUND_GAME_START
} SoundType;

void BEEP_SetFreq(uint16_t freq); // 设置蜂鸣器频率
void BEEP_On(void);               // 蜂鸣器开
void BEEP_Off(void);              // 蜂鸣器关

typedef enum
{            // 播放状态结构体
    STOPPED, // 停止播放
    PLAYING, // 正在播放
    PAUSED,  // 暂停播放
    LOADING  // 正在加载
} PlayState;

typedef struct
{
    char name[16];
    //const uint8_t (* code MusicSegments)[2]; // 歌曲片段指针数组
    const uint8_t (*MusicSegments)[2]; // 歌曲片段指针数组
    uint16_t segmentSizes[3];          // 每个片段的大小
    uint16_t speed;                    // 播放速度
} Song;

extern const uint16_t code FreqTable[];
extern const Song Songs[MAX_SONGS];
// 全局变量
extern volatile PlayState playerState; // 播放状态实例
extern unsigned char FreqSelect;       // 频率选择
extern unsigned char MusicSelect;      // 音符音高选择
extern unsigned char SongSelect;       // 歌曲选择

// 函数声明
void PlayMusic(void);                         // 播放音乐
static void playNote(unsigned char segIndex); // 播放音符
static void handleKeyEvents(void);            // 处理按键事件
//void CheckAndInitStorage(void);               // 检查并初始化存储器
void LoadMusicSegment(uint32_t addr);         // 加载音乐分段
uint8_t GetValidSegments(const Song *song);
static void HandlePlayingState(void);
static void HandlePauseState(void);

#endif
