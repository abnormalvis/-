#ifndef __MUSICTABLE_H__
#define __MUSICTABLE_H__
#include "stdint.h"
#include "Beep.h"
#define HARUHIKAGE_BPM 132
#define FLOWERTOWER_BPM 98
#define EMPTYBOX_BPM 152

#define ARRAY_LEN(arr) (sizeof(arr)/sizeof(arr[0]))
#define HARUHIKAGE1_NOTES (sizeof(Haruhikage1)/3)

#define BEAT_MS_1 (60000 / HARUHIKAGE_BPM)    // 节拍时间
#define BEAT_MS_2 (60000 / FLOWERTOWER_BPM)
#define BEAT_MS_3 (60000 / EMPTYBOX_BPM)
#define BAR_TO_MS(bar)    ((bar)*4*BEAT_MS)  // 每小节4拍
// #define BEAT_TO_MS(beat)  ((beat)*BEAT_MS)

// P：休止符  L：低音  M：中音  H：高音  下划线 升半音
#define P	0
#define L1	1
#define L1_	2
#define L2	3
#define L2_	4
#define L3	5
#define L4	6
#define L4_	7
#define L5	8
#define L5_	9
#define L6	10
#define L6_	11
#define L7	12
#define M1	13
#define M1_	14
#define M2	15
#define M2_	16
#define M3	17
#define M4	18
#define M4_	19
#define M5	20
#define M5_	21
#define M6	22
#define M6_	23
#define M7	24
#define H1	25
#define H1_	26
#define H2	27
#define H2_	28
#define H3	29
#define H4	30
#define H4_	31
#define H5	32
#define H5_	33
#define H6	34
#define H6_	35
#define H7	36
#define STOP 37


typedef struct 
{
    uint8_t track;      // 音符轨道
    uint32_t timing;    // 音符时间戳
} MusicNote;

extern const MusicNote code Haruhikage_Main[];

// 通过结构体计算数组长度
extern const uint32_t Haruhikage_Length;

// Haruhikage  - 春日影
extern const uint8_t code Haruhikage1[][2];
// extern const uint8_t code Haruhikage2[][2];
// extern const uint8_t code Haruhikage3[][2];
// Flowertower - 花之塔
extern const uint8_t code Flowertower[][2];
// Empty box   - 空之箱
extern const uint8_t code Emptybox[][2];

extern const MusicNote code Flowertower_Main[];
extern const MusicNote code EmptyBox_Main[];
extern const MusicNote code Haruhikage_Chorus[];
#endif