#ifndef __NOTE_H__
#define __NOTE_H__
void Note_Init(void);
void Note_Update(void);
void Note_Draw(void);
extern unsigned char track_position;	//音符轨道位置 1-2
extern unsigned char speed;				//音符下落速度 1-8
extern unsigned char x_position;		//音符水平位置 0-128

void note_spawn(unsigned char track);

#endif