#ifndef __MUSIC_H__
#define __MUSIC_H__
#include <REG52.H>

typedef struct {
    unsigned char status;   // bit0:playing, bit1:pause
    unsigned char index;
    unsigned int duration;
    unsigned char timerH;
    unsigned char timerL;
} MusicCtrl_t;

extern xdata MusicCtrl_t MusicCtrl;
extern const unsigned char code *currentTrack;

void Music_Init(const unsigned char code *track);
void Music_Play(void);
void Music_Stop(void);
void Music_Update(void);

extern const unsigned char code Flowertower[];
extern const unsigned char code Emptybox[];

#endif