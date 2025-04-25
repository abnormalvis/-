#ifndef OLED12864_H
#define OLED12864_H

#include <REG52.H>
#include <string.h>
#include <stdint.h>

// Ӳ����������
sbit OLED_SCL = P1^0;   // I2Cʱ����
sbit OLED_SDIN = P1^1;  // I2C������

// ��Ļ����
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PAGE_SIZE 8       // ÿҳ��������
#define MAX_COLUMN 128
#define MAX_PAGE 8

// ��������
#define OLED_CMD 0
#define OLED_DATA 1
#define OLED_CS_Clr() OLED_CS = 0
#define OLED_CS_Set() OLED_CS = 1

#define OLED_RST_Clr() OLED_RST = 0
#define OLED_RST_Set() OLED_RST = 1

#define OLED_DC_Clr() OLED_DC = 0
#define OLED_DC_Set() OLED_DC = 1

#define OLED_SCLK_Clr() OLED_SCL = 0
#define OLED_SCLK_Set() OLED_SCL = 1

#define OLED_SDIN_Clr() OLED_SDIN = 0
#define OLED_SDIN_Set() OLED_SDIN = 1

// OLEDģʽ����
// 0:4�ߴ���ģʽ
// 1:����8080ģʽ

#define SIZE 16
#define XLevelL 0x02
#define XLevelH 0x10
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64

// ��������
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Refresh(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);
void OLED_ShowChar(uint8_t x, uint8_t y, char chr, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, char *str, uint8_t size);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]);
void OLED_ScrollSetup(uint8_t dir, uint8_t start_page, uint8_t end_page, uint8_t speed);
void OLED_ScrollEnable(uint8_t en);
static void IIC_Start(void);
static void IIC_Stop(void);
static void Write_IIC_Byte(uint8_t dat);
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void Write_IIC_Data(uint8_t dat);
void Write_IIC_Command(uint8_t cmd);
#endif