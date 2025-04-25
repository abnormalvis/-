#ifndef OLED12864_H  
#define OLED12864_H

#include <REG52.H>    
#include "bmp.h"       
#include "Beep.h"
#include "stdint.h"    
#include <string.h>
#include <stdio.h>
    
#define OLED_MODE 0    // 0: IIC模式 1: SPI模式
#define SIZE 8         // 字体大小 8*8 12*12 16*16
#define XLevelL 0x00   // X坐标起始位置
#define XLevelH 0x10   // X坐标结束位置
#define Max_Column 128 
#define Max_Row 64     
#define X_WIDTH 128    
#define Y_WIDTH 64     

// 显示参数（根据SSD1306数据手册修正）
#define SSD1306_COL_ADDR 0x21  // 列地址设置命令
#define SSD1306_PAGE_ADDR 0x22 // 页地址设置命令
#define SCREEN_WIDTH 128       // 屏幕宽度
#define SCREEN_HEIGHT 64       // 屏幕高度

#define OLED_SCLK_Clr() OLED_SCLK = 0 // SCL拉低
#define OLED_SCLK_Set() OLED_SCLK = 1 // SCL拉高
#define OLED_SDIN_Clr() OLED_SDIN = 0 // SDA拉低
#define OLED_SDIN_Set() OLED_SDIN = 1 // SDA拉高

#define OLED_CMD 0  // 命令模式
#define OLED_DATA 1 // 数据模式

void OLED_Init(void);    // 初始化函数
void OLED_Clear(void);   // 清屏函数
void OLED_Refresh(void); // 刷新显示缓存

void OLED_SetPos(uint8_t x, uint8_t y); // 设置显示坐标
void OLED_DisplayOn(void);    // 开启显示
void OLED_DisplayOff(void);   // 关闭显示

void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);                      // 画点函数
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);           // 区域填充
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);             // 显示字符
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);    // 显示字符串
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]); // 显示位图
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_IIC_Start(void);        // I2C起始信号
void OLED_IIC_Stop(void);         // I2C停止信号
void OLED_IIC_WriteByte(uint8_t byte); // 发送单字节
extern const unsigned char code F6x8[][6];
extern const unsigned char code F8X16[];
#endif