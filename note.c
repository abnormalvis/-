// note.c
#include <REG52.H>
#include "note.h"
#include "music.h"
#include "oled.h"
#include "stdlib.h"
#include "stdlib.h" // 用于rand()函数

#define MAX_NOTES 5   // 最大同时显示的音符数
#define SPAWN_RATE 70 // 音符生成概率(0-100)

typedef struct
{
    unsigned char active; // 是否激活
    unsigned char x;      // X轴位置
    unsigned char track;  // 轨道编号
} Note;

Note notes[MAX_NOTES];
unsigned char current_speed = 6;

void Note_Init()
{
    unsigned char i;
    for (i = 0; i < MAX_NOTES; i++)
    {
        notes[i].active = 0;
    }
    current_speed = speed;
}

void Note_Update()
{
    // 只在音乐播放时更新
    unsigned char i;
    if (TR1 == 1 && currentTrack != NULL)
    {
        for (i = 0; i < MAX_NOTES; i++)
        {
            if (notes[i].active)
            {
                if (notes[i].x > current_speed)
                {
                    notes[i].x -= current_speed;
                }
                else
                {
                    notes[i].active = 0; // 移出屏幕
                }
            }
        }
    }
}

void Note_Spawn(unsigned char track)
{
    // 根据音乐节拍生成音符
    static unsigned char i;

    if (currentTrack[MusicCtrl.index] != 0xFF && (rand() % 100 < SPAWN_RATE))
    {
        for (i = 0; i < MAX_NOTES; i++)
        {
            if (!notes[i].active)
            {
                notes[i].x = 120;
                notes[i].track = track;
                notes[i].active = 1;
                break;
            }
        }
    }
}

void Note_Draw()
{
    unsigned char i;
    for (i = 0; i < MAX_NOTES; i++)
    {
        if (notes[i].active)
        {
            OLED_ShowString(notes[i].x, notes[i].track * 16, ":)", 16);
        }
    }
}