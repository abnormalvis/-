#ifndef __NOTE_H__
#define __NOTE_H__
void Note_Init(void);
void Note_Update(void);
void Note_Draw(void);
extern unsigned char track_position;	//�������λ�� 1-2
extern unsigned char speed;				//���������ٶ� 1-8
extern unsigned char x_position;		//����ˮƽλ�� 0-128

void note_spawn(unsigned char track);

#endif