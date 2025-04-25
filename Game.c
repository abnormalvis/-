#include "Game.h"
#include "SystemState.h"
#include "AT24C1024B.h"
#include "Beep.h"
#include "bmp.h"
#include "Musictable.h"
#include <string.h>
#include "Key.h"
#include "stdlib.h"
#include "stdint.h"
#include "oled12864.h"

// 全局游戏上下文
GameContext ctx;
static Note xdata note_pool[MAX_NOTES];           // 音符对象池
static TimeSlice xdata time_index[10];            // 时间索引表
static uint16_t note_index = 0;             // 音符池索引
static const uint8_t *current_music = NULL; // 当前音乐数据

// 轨道映射表
#define TRACK_MAP_DATA {1, 2, 3, 4, 1, 2, 3, 4}
static const uint8_t TRACK_MAP[8] = TRACK_MAP_DATA;
// static void GenerateNotesFromMusic(const uint8_t *data, uint32_t len);
volatile bool score_updated = false; // 分数更新标志
// #define MS_PER_BEAT (60000 / BPM)    // 每拍毫秒数
//  #define NOTE_SPEED (SCREEN_WIDTH / (MS_PER_BEAT * BEATS_PREVIEW)) // 提前2拍显示
//  建议增加BPM转换时间单位的宏定义
#define TIME_UNIT (60000 / ctx.bpm / 4) // 以四分音符为单位

float NOTE_SPEED = 0.0f;

static uint32_t beat_ms(uint16_t bpm)
{
    return 60000 / bpm;
}

// 添加音符到对象池
static void AddNoteToPool(Note note)
{
    int pos;                     // 音符池索引
    if (note_index >= MAX_NOTES) // 如果音符池满，直接返回
        return;

    pos = note_index - 1;                                   // 没有满， 从后往前找
    while (pos >= 0 && note_pool[pos].timing > note.timing) // 往前找，找到空位
    {
        note_pool[pos + 1] = note_pool[pos];
        pos--;
    }
    note_pool[++pos] = note;
    note_index++;
}
// 构建时间索引表
static void BuildTimeIndex(void)
{
    uint32_t current_start = 0, i, window_size;
    int idx = 0;

    window_size = beat_ms(ctx.bpm) * 2;
    memset(time_index, 0, sizeof(time_index));
    time_index[0].start_time = 0;

    for (i = 0; i < ctx.note_count && idx < 10; i++)
    {
        if (ctx.notes[i].timing > current_start + 500) /* 直接使用500代替WINDOW_SIZE */
        {
            time_index[idx].end_time = current_start + 500;
            time_index[idx].start_idx = i;

            if (++idx >= 10)
                break;

            current_start += 500;
            time_index[idx].start_time = current_start;
        }
    }
}
// 查找时间索引
static int FindStartIndex(uint32_t target_time)
{
    uint32_t search_start;
    int i, judge_range;
    
    judge_range = beat_ms(ctx.bpm) / 4;
    search_start = target_time > judge_range ? target_time - judge_range : 0;

    for (i = 0; i < 10; i++)
    {
        if (time_index[i].start_time <= search_start &&
            search_start < time_index[i].end_time)
        {
            return time_index[i].start_idx;
        }
    }
    return 0;
}

// 判定逻辑核心
static void JudgeNote(uint8_t track, uint32_t current_time)
{
    int start_idx = FindStartIndex(current_time);
    int judge_range = beat_ms(ctx.bpm) / 4;
    int min_diff = judge_range + 1;
    Note *best_note = NULL;
    int i; /* C90循环变量声明移至外部 */

    for (i = start_idx; i < ctx.note_count && i < ctx.current_index; i++)
    {
        Note *n = &ctx.notes[i];
        int diff;

        if (n->processed || !n->active || n->track != track)
            continue;

        diff = abs((int)(current_time - n->timing));
        /*
        int delta = (int)(current_time - n->timing);
        diff = delta > 0 ? delta : -delta;
        */
        if (diff > judge_range)
            continue;

        if (abs(diff) < min_diff)
        {
            min_diff = abs(diff);
            best_note = n;
        }
    }

    if (best_note)
    {
        best_note->processed = true;
        best_note->active = false;
        ctx.combo++;

        if (min_diff <= 50)
        { // PERFECT
            ctx.score += 1000;
            OLED_ShowString(90, 3, "PERFECT");
            ctx.judge_counts[JUDGE_PERFECT]++;
        }
        else if (min_diff <= 100)
        { // GOOD
            ctx.score += 500;
            OLED_ShowString(90, 3, "GOOD");
            ctx.judge_counts[JUDGE_GOOD]++;
        }
        else
        { // BAD
            ctx.score += 200;
            ctx.hp = ctx.hp > 5 ? ctx.hp - 5 : 0;
            OLED_ShowString(90, 3, "BAD");
            ctx.judge_counts[JUDGE_BAD]++;
        }
    }
    else
    {
        ctx.combo = 0;
        OLED_ShowString(90, 3, "MISS");
        if (ctx.combo < 10)
        { // 低连击时惩罚加重
            ctx.hp = ctx.hp > 10 ? ctx.hp - 10 : 0;
        }
        else
        {
            ctx.hp = ctx.hp > 5 ? ctx.hp - 5 : 0;
        }
        ctx.judge_counts[JUDGE_MISS]++;
    }

    // 连击恢复
    if (ctx.combo % COMBO_RECOVERY == 0 && ctx.hp < 100)
    {
        ctx.hp += 5;
        if (ctx.hp > 100)
            ctx.hp = 100;
    }
}

void Game_Init(void)
{
    uint8_t i;
    memset(&ctx, 0, sizeof(ctx));
    for (i = 0; i < JUDGE_LEVEL_MAX; i++)
    {
        ctx.judge_counts[i] = 0;
    }
    memset(note_pool, 0, sizeof(note_pool));
    note_index = 0;
    ctx.hp = 100;
    current_music = NULL;
}

uint16_t GetSongNoteCount(uint8_t songIndex)
{
    // uint8_t seg;
    uint16_t segmentSize, i;
    // static uint16_t noteCount;
    const Song *song = &Songs[songIndex];
    // noteCount = 0;
    static uint16_t totalNotes = 0;
    // for (seg = 0; seg < song->segmentCount; seg++)
    // {
    //     const uint8_t (*segment)[2] = song->MusicSegments[seg];
    //     segmentSize = song->segmentSizes[seg] / 2; // 每个音符占用2个字节

    //     for (i = 0; i < segmentSize; i++)
    //     {
    //         if (segment[i][0] == REST_NOTE)
    //         { // 休止符
    //             break;
    //         }
    //         totalNotes++;
    //     }
    // }
    const uint8_t (*segment)[2] = song->MusicSegments[0];
    segmentSize = song->segmentSizes[0] / 2;

    for (i = 0; i < segmentSize; i++)
    {
        if (segment[i][0] == 0xFF)
            break;
        totalNotes++;
    }

    return totalNotes;
}

// 初始化音符的时间戳
void Game_Start(uint8_t song_id)
{
    uint32_t music_length, accum_time = 0, i;
    uint8_t note, duration, interval;

    Game_Init(); // 重置游戏上下文
    switch (song_id)
    {
    case 0:
        current_music = (const uint8_t *)Haruhikage1; // 返回 Haruhikage1 数据
        ctx.bpm = HARUHIKAGE_BPM;
        break;
    case 1:
        current_music = (const uint8_t *)Emptybox; // 返回 Emptybox 数据
        ctx.bpm = EMPTYBOX_BPM;
        break;
    case 2:
        current_music = (const uint8_t *)Flowertower; // 返回 Flowertower 数据
        ctx.bpm = FLOWERTOWER_BPM;
        break;
    default:
        current_music = NULL;
        return;
    }

    NOTE_SPEED = 128.0f / ((60000.0f / ctx.bpm) * 2);
    music_length = GetSongNoteCount(song_id); // 获取音符数量
    note_index = 0;

    for (i = 0; i < music_length; i += 3)
    {
        note = current_music[i];
        duration = current_music[i + 1];
        interval = current_music[i + 2];

        if (note != 0xFF && note_index < MAX_NOTES)
        {
            note_pool[note_index].track = TRACK_MAP[note % 8];
            note_pool[note_index].timing = accum_time + JUDGE_ADVANCE;
            note_pool[note_index].active = false;
            note_pool[note_index].processed = false;
            // note_pool[note_index].type = (duration > 8) ? NOTE_HOLD : NOTE_NORMAL;   // 困难一点可以出现长条
            note_pool[note_index].type = NOTE_NORMAL; // 默认为普通音符
            note_index++;
        }
        accum_time += (duration + interval) * TIME_UNIT;
    }

    ctx.notes = note_pool; // 音符池
    ctx.note_count = note_index;
    ctx.start_time = GetSystemTick(); // 记录游戏开始时间
    BuildTimeIndex();                 // 构建时间索引
}

// 激活未来2秒内的音符
void Game_Update(void)
{
    uint32_t i, now;
    if (ctx.is_paused)
        return;

    now = GetSystemTick();

    // 激活未来2秒内的音符
    while (ctx.current_index < ctx.note_count &&
           note_pool[ctx.current_index].timing <= now + 2000)
    {
        note_pool[ctx.current_index].active = true;
        ctx.current_index++;
    }

    // 处理超时未击中的音符
    for (i = 0; i < ctx.current_index; i++)
    {
        Note *n = &note_pool[i];
        int judge_range;
        judge_range = beat_ms(ctx.bpm) / 4;
        if (n->active && !n->processed && (now - n->timing) > judge_range)
        {
            n->processed = true;
            ctx.combo = 0;
            ctx.judge_counts[JUDGE_MISS]++;
            ctx.hp = ctx.hp > 10 ? ctx.hp - 10 : 0;
        }
    }

    // 游戏结束检测
    if (ctx.hp == 0)
    {
        SaveHighScore(ctx.score);            // 保存最高分
        SystemState_Transition(SYSTEM_MENU); // 切换到菜单界面
    }
}

// 处理按键输入
void Game_HandleInput(uint8_t key)
{
    static uint32_t last_input[4] = {0, 0, 0, 0};
    uint8_t track;
    uint32_t now;

    if (key == KEY_EXIT)
    {
        SaveHighScore(ctx.score);
        SystemState_Transition(SYSTEM_MENU);
        return;
    }

    if (key < KEY_TRACK1 || key > KEY_TRACK4)
        return;

    track = key - KEY_TRACK1 + 1;
    now = GetSystemTick();

    /* 输入冷却检测（100ms） */
    if (now - last_input[track - 1] < 100)
        return;
    last_input[track - 1] = now;

    JudgeNote(track, now);
}

// 保存历史成绩最高分
void SaveHighScore(uint32_t score)
{
    uint8_t buf[4];
    uint32_t HI;
    buf[0] = (score >> 24) & 0xFF;
    buf[1] = (score >> 16) & 0xFF;
    buf[2] = (score >> 8) & 0xFF;
    buf[3] = score & 0xFF;
    // 先读取历史最高分
    HI = LoadHighScore();
    if (HI < score)
    {
        AT24C1024_Write(0x0000, buf, 4); // 主存储
        AT24C1024_Write(0x0400, buf, 4); // 备份存储
    }
}

// 读取历史成绩
uint32_t LoadHighScore(void)
{
    uint8_t buf1[4], buf2[4];
    AT24C1024_Read(0x0000, buf1, 4);
    AT24C1024_Read(0x0400, buf2, 4);

    if (memcmp(buf1, buf2, 4) == 0)
    {
        return (buf1[0] << 24) | (buf1[1] << 16) | (buf1[2] << 8) | buf1[3];
    }
    return 0; // 校验失败
}

void ShowResult(void)
{
    uint8_t key;
    uint32_t HI;
    HI = LoadHighScore();
    // oled屏幕展示游玩结果
    OLED_ShowString(0, 0, "Score:");   // 显示文本标签
    OLED_ShowNum(40, 0, ctx.score, 8); // 显示8位分数

    OLED_ShowString(60, 1, "HI:");
    OLED_ShowNum(80, 1, HI, 8);

    OLED_ShowString(0, 1, "Perfect:");
    OLED_ShowNum(40, 1, ctx.judge_counts[JUDGE_PERFECT], 3);

    OLED_ShowString(0, 2, "Good:");
    OLED_ShowNum(40, 2, ctx.judge_counts[JUDGE_GOOD], 3);

    OLED_ShowString(0, 3, "Bad:");
    OLED_ShowNum(40, 3, ctx.judge_counts[JUDGE_BAD], 3);

    OLED_ShowString(0, 4, "Miss:");
    OLED_ShowNum(40, 4, ctx.judge_counts[JUDGE_MISS], 3);
    // 按键处理循环
    while (1)
    {
        // 检测按键输入
        key = Get_Key();
        if (key == KEY_CONFIRM)
        {
            Menu_TransitionTo(MENU_MAIN); // 使用菜单状态机返回主菜单
            break;
        }
    }
}

// 绘制游戏画面
void Game_Draw(void)
{
    const float NOTE_SPEED = 128.0f / ((60000.0f / ctx.bpm) * 2);
    uint32_t now = GetSystemTick();
    uint8_t i;
    OLED_Clear();

    // 绘制状态栏
    OLED_ShowString(0, 0, "COMBO:");
    OLED_ShowNum(48, 0, ctx.combo, 3);
    OLED_ShowString(80, 0, "HP:");
    OLED_ShowNum(104, 0, ctx.hp, 3);

    // 绘制判定线
    OLED_DrawBMP(JUDGE_LINE_X, 0, JUDGE_LINE_X + 3, 7, gImage_PanDingXian);

    for (i = 0; i < ctx.current_index; i++)
    {
        Note *n = &note_pool[i];
        if (n->active && !n->processed)
        {
            // 计算音符X坐标（随时间移动）
            int x = JUDGE_LINE_X - (now - n->timing) * NOTE_SPEED;

            // 仅在可见范围内绘制
            if (x >= 0 && x < SCREEN_WIDTH - 4) // 假设位图宽度4像素
            {
                // 根据轨道号计算Y坐标
                uint8_t y = 0;
                switch (n->track)
                {
                case 1:
                    y = 16;
                    break; // 轨道1在屏幕1/4处
                case 2:
                    y = 32;
                    break; // 轨道2在屏幕1/2处
                case 3:
                    y = 48;
                    break; // 轨道3在屏幕3/4处
                case 4:
                    y = 56;
                    break; // 轨道4在底部
                default:
                    y = 0;
                }

                // 绘制音符位图（8x8像素）
                // 改为绘制宽为2列高为8个像素点，不绘制位图
                OLED_DrawBMP(x, y, x + 7, y + 7, gImage_jianxing); // 假设已定义8x8音符位图
            }
        }
    }
    OLED_Refresh();
}