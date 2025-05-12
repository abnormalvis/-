#include "bmp.h"
#include "Beep.h"
#include "key.h"
#include "Delay.h"
#include "AT24C1024B.h"
#include "Musictable.h"
#include "oled12864.h"
#include "menu.h"
#include "Game.h"
#include "Timer0.h"
#include "Timer1.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "oledfont.h"
#define BUFFER_SIZE 512 // 缓冲区大小
volatile PlayState playerState = STOPPED;  // 初始化为暂停播放状态
uint8_t xdata MusicBuffer[2][BUFFER_SIZE]; // 双缓冲
volatile uint8_t current_buf = 0;          // 当前缓冲区索引
uint32_t current_addr = 0;                 // 当前播放地址
uint16_t buf_index = 0;                    // 当前缓冲区索引
uint8_t SongSelect = 0;                    // 当前曲目
unsigned char FreqSelect = 0;

const Song Songs[MAX_SONGS] = {
    {"Haruhikage", Haruhikage1, {sizeof(Haruhikage1) / sizeof(Haruhikage1[0]), 0, 0}, 135},
    {"Emptybox", Emptybox, {sizeof(Emptybox) / sizeof(Emptybox[0]), 0, 0}, 160},
    {"Flowertower", Flowertower, {sizeof(Flowertower) / sizeof(Flowertower[0]), 0, 0}, 85}};

const uint16_t code FreqTable[] = {
    0, // 休止符  3个8度的音高
    63777, 63872, 63969, 64054, 64140, 64216, 64291, 64360, 64426, 64489, 64547, 64603,
    64655, 64704, 64751, 64795, 64837, 64876, 64913, 64948, 64981, 65012, 65042, 65070,
    65095, 65120, 65144, 65166, 65186, 65206, 65225, 65242, 65259, 65274, 65289, 65303};

void BEEP_SetFreq(uint16_t freq)
{
    uint32_t reload; // 重装载值
    if (freq == 0)
    { // 如果频率为0，则关闭定时器
        TR1 = 0;
        return;
    }
    reload = 65536 - (11059200L / (freq * 12 * 2)); // 计算重装载值
    TH1 = reload >> 8;                              // 设置定时器1的寄存器
    TL1 = reload;
    TR1 = 1; // 启动定时器1
}

void BEEP_On(void)
{
    TR1 = 1; // 启动定时器1
}

void BEEP_Off(void)
{
    TR1 = 0;    // 关闭定时器1
    BUZZER = 0; // 确保蜂鸣器静音
}

// void BEEP_Play(SoundType sound)
// {
//     uint8_t i;
//     switch (sound)
//     {
//     case SOUND_PAUSE:
//         // 短促提示音 大概 800Hz 200ms
//         BEEP_SetFreq(800);
//         BEEP_On();
//         Delay(200);
//         BEEP_Off();
//         break;

//     case SOUND_GAME_OVER:
//         // 进行一个三连音提示
//         for (i = 0; i < 3; i++)
//         {
//             BEEP_SetFreq(600);
//             BEEP_On();
//             Delay(300);
//             BEEP_Off();
//             Delay(150);
//         }
//         break;

//     case SOUND_MENU:
//         // 菜单选择音 1200Hz 100ms
//         BEEP_SetFreq(1200);
//         BEEP_On();
//         Delay(100);
//         BEEP_Off();
//         break;
//     case SOUND_GAME_START:
//         // 游戏开始 600Hz 50ms
//         BEEP_SetFreq(600);
//         BEEP_On();
//         Delay(50);
//         BEEP_Off();
//         break;
//     }
// }

/*
预加载应该采用非阻塞式，否则会影响播放
播放主循环完全由系统状态驱动
暂停状态单独处理显示刷新
错误状态立即终止播放
*/

void LoadMusicSegment(uint32_t addr)
{
    // uint8_t tmp_buf, ea_save; 
    // tmp_buf = !current_buf;
    // AT24C1024_Read(addr, MusicBuffer[tmp_buf], BUFFER_SIZE);

    //
    // ea_save = EA;
    // EA = 0;
    // current_buf = tmp_buf;
    // EA = ea_save;
    const uint8_t* music_data = Musictable_GetSongData(SongSelect);
    memcpy(MusicBuffer[tmp_buf], music_data, BUFFER_SIZE);
}

uint8_t GetValidSegments(const Song *song)
{
    uint8_t count = 0;
    while (count < 3 && song->segmentSizes[count] != 0)
    {
        count++;
    }
    return count;
}

// 检查并且初始化存储器
void CheckAndInitStorage()
{
    // uint8_t i, seg;
    // uint32_t addr;
    // const uint8_t sig[4] = {0xAA, 0x55, 0xAA, 0x55};

    // if (!CheckDataValid())
    // {                                   // 先检测数据的有效性
    //     EA = 0;                         // 关中断
    //     for (i = 0; i < MAX_SONGS; i++) // 遍历曲库
    //     {
    //         const Song *song = &Songs[i];
    //         for (seg = 0; seg < GetValidSegments(song); seg++)
    //         {
    //             // 遍历曲库的每个片段
    //             if (song->segmentSizes[seg] == 0)
    //             {
    //                 break;
    //             }
    //             // 写入曲库首地址
    //             addr = 0;
    //             AT24C1024_Write(addr, (uint8_t *)song->MusicSegments[seg], song->segmentSizes[seg]);
    //             addr += song->segmentSizes[seg]; // 地址偏移
    //         }
    //     }
    //     // 写入校验标记
    //     AT24C1024_Write(AT24C1024_SIZE - 4, sig, 4);
    //     EA = 1; // 开中断
    // }
}

// 播放控制

// 主播放逻辑
void PlayMusic()
{
    // 开始播放前先初始化定时器1
    Timer1Init();
    // 然后检测存储器
    CheckAndInitStorage();
    // 接着加载
    LoadMusicSegment(current_addr);

    // 状态转换为播放状态
    SystemState_Transition(SYSTEM_MUSIC_PLAYING);

    while (g_state.current_state == SYSTEM_MUSIC_PLAYING)
    { // 当播放没有结束就循环处理用户的按键游玩输入
        handleKeyEvents();

        // 根据系统状态控制播放流程
        switch (g_state.current_state)
        {
        case SYSTEM_MUSIC_PLAYING:
            HandlePlayingState();
            break;

        case SYSTEM_PAUSE:
            HandlePauseState();
            break;

        case SYSTEM_ERROR:
            TR1 = 0; // 立即停止播放
            return;
        }
    }
}

// 处理
static void HandlePlayingState(void)
{
    uint8_t validSegments;
    uint16_t note, duration, freq;
    const uint8_t (*currentSegment)[2];
    // 缓冲区边界检测
    if (buf_index >= BUFFER_SIZE / (2 * sizeof(uint16_t)))
    { // 如果缓冲区已满，则切换缓冲区
        SystemState_Transition(LOADING);
        current_buf = !current_buf;
        current_addr += BUFFER_SIZE;
        current_addr %= (AT24C1024_SIZE - BUFFER_SIZE);
        LoadMusicSegment(current_addr);
        buf_index = 0;
        SystemState_Transition(SYSTEM_MUSIC_PLAYING);
    }

    // 数据结束检测
    if (MusicBuffer[!current_buf][buf_index * 2] == 0xFF)
    {
        SystemState_Transition(SYSTEM_MENU); // 返回菜单
        return;
    }

    // PlayNote();
    validSegments = GetValidSegments(&Songs[SongSelect]);
    
    currentSegment = Songs[SongSelect].MusicSegments[validSegments - 1];
    note = MusicBuffer[current_buf][buf_index * 2];
    duration = MusicBuffer[current_buf][buf_index * 2 + 1];

    if (note == REST_NOTE)
    { // 休止符
        BEEP_Off();
        Delay(duration);
        return;
    }

    freq = FreqTable[note];
    BEEP_SetFreq(freq);
    BEEP_On();
    Delay(duration);
    BEEP_Off();
    buf_index++;

    // 预加载判断
    if (buf_index > BUFFER_SIZE / 2 - (BUFFER_SIZE / 10))
    {
        EA = 0;
        LoadMusicSegment(current_addr + BUFFER_SIZE);
        EA = 1;
    }
}

// 暂停游玩和继续游玩按键事件切换
static void handleKeyEvents()
{
    static uint8_t lastKey;
    uint8_t key = Key();

    if (key != lastKey)
    {
        switch (key)
        {
            // case KEY_NEXT_SONG:
            //     // 切歌
            //     if (g_state.current_state != SYSTEM_MUSIC_PLAYING)
            //     { // 只有在暂停状态才允许切歌
            //         SongSelect = (SongSelect + 1) % MAX_SONGS;
            //         current_addr = 0; // 切歌后重置播放地址和播放位置
            //         buf_index = 0;
            //         SystemState_Transition(LOADING);
            //     }
            //     break;

        case KEY_PAUSE:
            // 检测当前状态是否为游玩状态
            if (g_state.current_state == SYSTEM_MUSIC_PLAYING)
            { // 如果是游玩状态，则切换到暂停状态
                SystemState_Transition(SYSTEM_PAUSE);
            }
            else if (g_state.current_state == SYSTEM_PAUSE)
            { // 如果是暂停状态，则切换到游玩状态
                SystemState_Transition(SYSTEM_MUSIC_PLAYING);
            }
            break;
        }
        lastKey = key;
    }
}

// 处理暂停和继续游戏的回调函数
static void HandlePauseState(void)
{
    TR1 = 0; // 先关闭定时器
    while (g_state.current_state == SYSTEM_PAUSE)
    { // 循环等待按键
        handleKeyEvents();
        // 暂停时保持刷新显示
        OLED_Refresh();
    }
    if (g_state.current_state == SYSTEM_MUSIC_PLAYING)
    { // 继续游戏
        TR1 = 1;
    }
}
