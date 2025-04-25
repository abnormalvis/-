#include <REG52.H>
#include "Timer0.h"
#include "Delay.h"
#include "note.h"
#include "oled.h"
#include "oledfont.h"
#include "key.h"
#include "Timer1.h"
#include "music.h"
void Timer1_Init(void)
{
    TMOD &= 0x0F;       // Clear T1 config
    TMOD |= 0x10;       // Mode 1 (16-bit)
    ET1 = 1;            // Enable interrupt
    PT1 = 1;            // High priority
}

void Timer1_ISR(void) interrupt 3 using 2  // Dedicated register bank
{
    if(MusicCtrl.status & 0x01) { // Check playing flag
        TL1 = MusicCtrl.timerL;   // Directly use pre-calculated
        TH1 = MusicCtrl.timerH;   // values from music module
        Buzzer = !Buzzer;         // Toggle buzzer
    } else {
        Buzzer = 0;               // Silence
    }
}