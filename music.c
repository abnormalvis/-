#include "music.h"
#include "Timer0.h"
#include "Timer1.h"
typedef unsigned int uint32_t;
xdata MusicCtrl_t MusicCtrl;
const unsigned char code *currentTrack;

const unsigned short code FreqTable[] = {
    // 简化的频率表（保留关键频率）
    0,
    63777, 63872, 63969, 64054, 64140, 64216, // L1-L6
    64291, 64360, 64426, 64489, 64547, 64603, // M1-M6
    64655, 64704, 64751, 64795, 64837, 64876  // H1-H6
};

void Music_Init(const unsigned char code *track)
{
    currentTrack = track;
    MusicCtrl.index = 0;
    MusicCtrl.status = 0;
    TR1 = 0;  // Stop timer initially
}

void Music_Play(void)
{
    if(currentTrack && (MusicCtrl.status & 0x01) == 0) {
        MusicCtrl.status |= 0x01; // Set playing flag
        TR1 = 1;                  // Start timer
    }
}

void Music_Stop(void)
{
    MusicCtrl.status &= ~0x01; // Clear playing flag
    TR1 = 0;                   // Stop timer
    Buzzer = 0;                // Silence
}

void Music_Update(void)
{
    static uint32_t lastTick = 0;
    
    if((MusicCtrl.status & 0x01) && currentTrack) {
        if(SystemTick - lastTick >= MusicCtrl.duration) {
            // 更新音符
            unsigned char note = currentTrack[MusicCtrl.index];
            if(note == 0xFF) { // End of track
                Music_Stop();
                return;
            }
            
            // 直接计算定时器值
            MusicCtrl.timerH = FreqTable[note] >> 8;
            MusicCtrl.timerL = FreqTable[note] & 0xFF;
            
            // 计算持续时间（使用移位优化）
            MusicCtrl.duration = (300 * currentTrack[MusicCtrl.index+1]) >> 2;
            
            MusicCtrl.index += 2;
            lastTick = SystemTick;
        }
    }
}