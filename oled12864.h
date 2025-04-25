#ifndef OLED12864_H  
#define OLED12864_H

#include <REG52.H>    
#include "bmp.h"       
#include "Beep.h"
#include "stdint.h"    
#include <string.h>
#include <stdio.h>
    
#define OLED_MODE 0    // 0: IICģʽ 1: SPIģʽ
#define SIZE 8         // �����С 8*8 12*12 16*16
#define XLevelL 0x00   // X������ʼλ��
#define XLevelH 0x10   // X�������λ��
#define Max_Column 128 
#define Max_Row 64     
#define X_WIDTH 128    
#define Y_WIDTH 64     

// ��ʾ����������SSD1306�����ֲ�������
#define SSD1306_COL_ADDR 0x21  // �е�ַ��������
#define SSD1306_PAGE_ADDR 0x22 // ҳ��ַ��������
#define SCREEN_WIDTH 128       // ��Ļ���
#define SCREEN_HEIGHT 64       // ��Ļ�߶�

#define OLED_SCLK_Clr() OLED_SCLK = 0 // SCL����
#define OLED_SCLK_Set() OLED_SCLK = 1 // SCL����
#define OLED_SDIN_Clr() OLED_SDIN = 0 // SDA����
#define OLED_SDIN_Set() OLED_SDIN = 1 // SDA����

#define OLED_CMD 0  // ����ģʽ
#define OLED_DATA 1 // ����ģʽ

void OLED_Init(void);    // ��ʼ������
void OLED_Clear(void);   // ��������
void OLED_Refresh(void); // ˢ����ʾ����

void OLED_SetPos(uint8_t x, uint8_t y); // ������ʾ����
void OLED_DisplayOn(void);    // ������ʾ
void OLED_DisplayOff(void);   // �ر���ʾ

void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);                      // ���㺯��
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);           // �������
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);             // ��ʾ�ַ�
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);    // ��ʾ�ַ���
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]); // ��ʾλͼ
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_IIC_Start(void);        // I2C��ʼ�ź�
void OLED_IIC_Stop(void);         // I2Cֹͣ�ź�
void OLED_IIC_WriteByte(uint8_t byte); // ���͵��ֽ�
extern const unsigned char code F6x8[][6];
extern const unsigned char code F8X16[];
#endif