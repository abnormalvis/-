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
static Note xdata note_pool[MAX_NOTES]; // 音符对象池
static TimeSlice xdata time_index[10];  // 时间索引表
static uint16_t note_index = 0;         // 音符池索引
uint8_t xdata current_music_buf[1024];
static uint8_t *current_music = current_music_buf;
uint32_t high_score = 0;
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
            OLED_ShowString(90, 3, "PERFECT", 8);
            ctx.judge_counts[JUDGE_PERFECT]++;
        }
        else if (min_diff <= 100)
        { // GOOD
            ctx.score += 500;
            OLED_ShowString(90, 3, "GOOD", 8);
            ctx.judge_counts[JUDGE_GOOD]++;
        }
        else
        { // BAD
            ctx.score += 200;
            ctx.hp = ctx.hp > 5 ? ctx.hp - 5 : 0;
            OLED_ShowString(90, 3, "BAD", 8);
            ctx.judge_counts[JUDGE_BAD]++;
        }
    }
    else
    {
        ctx.combo = 0;
        OLED_ShowString(90, 3, "MISS", 8);
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

const uint8_t *Musictable_GetSongData(uint8_t songIndex)
{
    switch (songIndex)
    {
    case 0:
        return Haruhikage1;
    case 1:
        return Emptybox;
    case 2:
        return Flowertower;
    default:
        return NULL;
    }
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
    Note temp_note;
    const uint8_t *music_data;
    const uint16_t bpm_table[] = {HARUHIKAGE_BPM, EMPTYBOX_BPM, FLOWERTOWER_BPM};

    Game_Init();
    music_data = Musictable_GetSongData(song_id);

    if (music_data == NULL)
    {
        SystemState_Transition(SYSTEM_MENU);
        return;
    }

    ctx.bpm = (song_id < 3) ? bpm_table[song_id] : 120;

    NOTE_SPEED = 128.0f / ((60000.0f / ctx.bpm) * 2);
    current_music = (uint8_t *)music_data;

    music_length = GetSongNoteCount(song_id);
    for (i = 0; i < music_length && note_index < MAX_NOTES; i += 3)
    {
        uint8_t current_note = current_music[i];
        if (current_note == 0xFF)
            continue;

        temp_note.track = TRACK_MAP[current_note % 8];
        temp_note.timing = accum_time + JUDGE_ADVANCE;
        temp_note.active = 0;
        temp_note.processed = 0;
        temp_note.type = NOTE_NORMAL;

        note_pool[note_index] = temp_note;
        accum_time += (current_music[i + 1] + current_music[i + 2]) * TIME_UNIT;
        note_index++;
    }

    ctx.notes = note_pool;
    ctx.note_count = note_index;
    ctx.start_time = GetSystemTick();
    BuildTimeIndex();
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
    static uint32_t last_input = 0;
    uint32_t now = GetSystemTick();

    if (key == KEY_PAUSE)
    { // 暂停游戏
        SystemState_Transition(SYSTEM_PAUSE);
        return;
    }

    if (key == KEY_CONFIRM)
    { // 游玩键
        if (now - last_input > 100)
        {                      // 100ms冷却
            JudgeNote(1, now); // 所有音符映射到单轨道
            last_input = now;
        }
    }
}

// 保存历史成绩最高分
void SaveHighScore(uint32_t score)
{
    if (score > high_score)
    {
        high_score = score;
    }
}

// 读取历史成绩
uint32_t LoadHighScore(void)
{
    return high_score;
}

void ShowResult(void)
{
    uint8_t key, y;
    uint32_t HI = LoadHighScore();
    OLED_Clear();

    // 分数显示（统一使用8号字体）
    OLED_ShowString(0, 0, "Score:", 8);
    OLED_ShowNum(36, 0, ctx.score, 6, 8); // x=36 (6*6=36)

    // 高分显示
    OLED_ShowString(0, 1, "HI:", 8);
    OLED_ShowNum(24, 1, HI, 8, 8); // 8位数字

    // 判定统计
    y = 2;
    OLED_ShowString(0, y, "Perfect:", 8);
    OLED_ShowNum(48, y, ctx.judge_counts[JUDGE_PERFECT], 3, 8);
    y++;

    OLED_ShowString(0, y, "Good:", 8);
    OLED_ShowNum(36, y, ctx.judge_counts[JUDGE_GOOD], 3, 8);
    y++;

    OLED_ShowString(0, y, "Bad:", 8);
    OLED_ShowNum(24, y, ctx.judge_counts[JUDGE_BAD], 3, 8);
    y++;

    OLED_ShowString(0, y, "Miss:", 8);
    OLED_ShowNum(30, y, ctx.judge_counts[JUDGE_MISS], 3, 8);

    OLED_Refresh();

    // 等待确认
    while (1)
    {
        key = Get_Key();
        if (key == KEY_CONFIRM)
        {
            Menu_TransitionTo(MENU_MAIN);
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

    // 绘制状态栏（统一使用8号字体）
    OLED_ShowString(0, 0, "COMBO:", 8);
    OLED_ShowNum(36, 0, ctx.combo, 3, 8); // x=36 (6字符*6像素)

    OLED_ShowString(72, 0, "HP:", 8);  // x=72
    OLED_ShowNum(90, 0, ctx.hp, 3, 8); // x=90

    // 绘制判定线（大写字母I代替）
    OLED_ShowChar(JUDGE_LINE_X, 0, 'I', 8); // 0表示第0页（0-7行）

    // 绘制活动音符
    for (i = 0; i < ctx.current_index; i++)
    {
        Note *n = &note_pool[i];
        if (n->active && !n->processed)
        {
            // 计算音符X坐标（随时间移动）
            int x = JUDGE_LINE_X - (now - n->timing) * NOTE_SPEED;

            if (x >= 0 && x < SCREEN_WIDTH - 6) // 6像素宽字符
            {
                // 计算页地址（y坐标转页）
                uint8_t page = 0;
                switch (n->track)
                {
                case 1:
                    page = 2;
                    break; // 16/8=2
                case 2:
                    page = 4;
                    break; // 32/8=4
                case 3:
                    page = 6;
                    break; // 48/8=6
                case 4:
                    page = 7;
                    break; // 56/8=7
                }

                // 显示字符G代替音符
                OLED_ShowChar(x, page, 'G', 8);
            }
        }
    }
    OLED_Refresh();
}