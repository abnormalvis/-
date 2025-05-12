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
#define KEY_PAUSE 2 // ���ֲ�����ͣ��
#endif
#define BEEP_ON() (Buzzer = 1)  // ��������
#define BEEP_OFF() (Buzzer = 0) // ��������
extern uint32_t current_addr;        // ��ǰ���ŵ�ַ
extern volatile uint8_t current_buf; // ��ǰ���Ż�����
typedef enum
{                    // �������ͽṹ��
    SOUND_PAUSE,     // ��ͣ����
    SOUND_GAME_OVER, // ��Ϸ����
    SOUND_MENU,       // �˵�
    SOUND_GAME_START
} SoundType;

void BEEP_SetFreq(uint16_t freq); // ���÷�����Ƶ��
void BEEP_On(void);               // ��������
void BEEP_Off(void);              // ��������

typedef enum
{            // ����״̬�ṹ��
    STOPPED, // ֹͣ����
    PLAYING, // ���ڲ���
    PAUSED,  // ��ͣ����
    LOADING  // ���ڼ���
} PlayState;

typedef struct
{
    char name[16];
    //const uint8_t (* code MusicSegments)[2]; // ����Ƭ��ָ������
    const uint8_t (*MusicSegments)[2]; // ����Ƭ��ָ������
    uint16_t segmentSizes[3];          // ÿ��Ƭ�εĴ�С
    uint16_t speed;                    // �����ٶ�
} Song;

extern const uint16_t code FreqTable[];
extern const Song Songs[MAX_SONGS];
// ȫ�ֱ���
extern volatile PlayState playerState; // ����״̬ʵ��
extern unsigned char FreqSelect;       // Ƶ��ѡ��
extern unsigned char MusicSelect;      // ��������ѡ��
extern unsigned char SongSelect;       // ����ѡ��

// ��������
void PlayMusic(void);                         // ��������
static void playNote(unsigned char segIndex); // ��������
static void handleKeyEvents(void);            // �������¼�
//void CheckAndInitStorage(void);               // ��鲢��ʼ���洢��
void LoadMusicSegment(uint32_t addr);         // �������ֶַ�
uint8_t GetValidSegments(const Song *song);
static void HandlePlayingState(void);
static void HandlePauseState(void);

#endif
