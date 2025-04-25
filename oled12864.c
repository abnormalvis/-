#include "oled12864.h"
#include "oledfont.h"
#include "Delay.h"
#include "stdint.h"

#define OLED_BUF_SIZE 128
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PAGE_SIZE 8
#define OLED_CMD 0
#define OLED_DATA 1

sbit OLED_RST = P1 ^ 2; // ȡ��ע�Ͳ�ȷ��Ӳ������

uint8_t xdata OLEDBuffer[SCREEN_HEIGHT / PAGE_SIZE][SCREEN_WIDTH];

/*---- I2C�������� ----*/
void IIC_Start()
{
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

void IIC_Stop()
{
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

// ע��51��Ƭ��IOΪ׼˫��ڣ�ACK��������ҪӲ��֧��
void IIC_Wait_Ack()
{
	OLED_SCLK_Set();
	// while(OLED_SDA_Read()); // ��Ҫʵ��OLED_SDA_Read()
	OLED_SCLK_Clr();
}

void Write_IIC_Byte(uint8_t IIC_Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK_Clr();
		if (IIC_Byte & 0x80)
			OLED_SDIN_Set();
		else
			OLED_SDIN_Clr();
		IIC_Byte <<= 1;
		OLED_SCLK_Set();
	}
}

/*---- OLED�������� ----*/
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	IIC_Start();
	Write_IIC_Byte(0x78);			   // I2C��ַ
	Write_IIC_Byte(cmd ? 0x40 : 0x00); // ����/�����ʶ
	Write_IIC_Byte(dat);
	IIC_Stop();
}

/*---- Ӳ����λ ----*/
void OLED_Reset()
{
	OLED_RST = 0;
	Delay(100);
	OLED_RST = 1;
	Delay(100);
}

/*---- ��ʼ������ ----*/
void OLED_Init(void)
{
	OLED_Reset(); // ���Ӳ����λ

	OLED_WR_Byte(0xAE, OLED_CMD); // �ر���ʾ
	// ��ʼ����������
	OLED_WR_Byte(0xD5, OLED_CMD);
	OLED_WR_Byte(0x80, OLED_CMD); // ʱ�ӷ�Ƶ
	OLED_WR_Byte(0xA8, OLED_CMD);
	OLED_WR_Byte(0x3F, OLED_CMD); // ���ñ���
	OLED_WR_Byte(0xD3, OLED_CMD);
	OLED_WR_Byte(0x00, OLED_CMD); // ��ʾƫ��
	OLED_WR_Byte(0x40, OLED_CMD); // ��ʼ��
	OLED_WR_Byte(0x8D, OLED_CMD);
	OLED_WR_Byte(0x14, OLED_CMD); // ��ɱ�
	OLED_WR_Byte(0x20, OLED_CMD);
	OLED_WR_Byte(0x00, OLED_CMD); // ˮƽѰַ
	OLED_WR_Byte(0xA1, OLED_CMD); // ����ӳ��
	OLED_WR_Byte(0xC8, OLED_CMD); // ɨ�跽��
	OLED_WR_Byte(0xDA, OLED_CMD);
	OLED_WR_Byte(0x12, OLED_CMD); // COM����
	OLED_WR_Byte(0x81, OLED_CMD);
	OLED_WR_Byte(0xCF, OLED_CMD); // �Աȶ�
	OLED_WR_Byte(0xD9, OLED_CMD);
	OLED_WR_Byte(0xF1, OLED_CMD); // Ԥ���
	OLED_WR_Byte(0xDB, OLED_CMD);
	OLED_WR_Byte(0x30, OLED_CMD); // VCOMH
	OLED_WR_Byte(0xA4, OLED_CMD); // ��ʾ�ָ�
	OLED_WR_Byte(0xA6, OLED_CMD); // ������ʾ
	OLED_WR_Byte(0xAF, OLED_CMD); // ������ʾ

	OLED_Clear(); // ��������ʼ��������
	OLED_Refresh();
}

/*---- �������� ----*/
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	if (x >= SCREEN_WIDTH)
		x = 0;
	if (y >= (SCREEN_HEIGHT / PAGE_SIZE))
		y = 0; // �߽���

	OLED_WR_Byte(0xB0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte(x & 0x0F, OLED_CMD);
}

/*---- �������� ----*/
void OLED_Clear(void)
{
	uint8_t page, col;
	for (page = 0; page < 8; page++)
	{
		for (col = 0; col < 128; col++)
		{
			OLEDBuffer[page][col] = 0x00;
		}
	}
}

/*---- ��ʾˢ�� ----*/
void OLED_Refresh(void)
{
	uint8_t page, col;
	for (page = 0; page < 8; page++)
	{
		OLED_Set_Pos(0, page);
		for (col = 0; col < 128; col++)
		{
			OLED_WR_Byte(OLEDBuffer[page][col], OLED_DATA);
		}
	}
}

/*---- �ַ���ʾ�����߽��飩----*/
void OLED_ShowChar(uint8_t x, uint8_t y, char chr, uint8_t size)
{
	uint8_t c, i;
	// �߽���
	if (x >= SCREEN_WIDTH - 8 || y >= SCREEN_HEIGHT / PAGE_SIZE)
		return;
	if (size == 16 && y >= 7)
		return; // ��ֹԽ��
	c = chr - ' ';
	if (size == 16)
	{
		for (i = 0; i < 8; i++)
		{
			OLEDBuffer[y][x + i] = F8X16[c * 16 + i];
			OLEDBuffer[y + 1][x + i] = F8X16[c * 16 + i + 8];
		}
	}
	else
	{
		for (i = 0; i < 6; i++)
		{
			OLEDBuffer[y][x + i] = F6x8[c][i];
		}
	}
}

/*---- �ַ�����ʾ ----*/
void OLED_ShowString(uint8_t x, uint8_t y, char *str, uint8_t size)
{
	while (*str)
	{
		OLED_ShowChar(x, y, *str++, size);
		x += (size == 16) ? 8 : 6;
		if (x >= SCREEN_WIDTH - ((size == 16) ? 8 : 6))
		{
			x = 0;
			y += (size == 16) ? 2 : 1;
			if (y >= SCREEN_HEIGHT / PAGE_SIZE)
				break;
		}
	}
}

/*---- ������ʾ����ȫ�汾��----*/
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
	char str[11] = {0};			 // ���֧��10λ����
	uint8_t idx, i;
	len = (len > 10) ? 10 : len; // ��ֹ���

	for (i = 0; i < len; i++)
		str[i] = '0';
	idx = len - 1;
	while (num && idx >= 0)
	{
		str[idx--] = (num % 10) + '0';
		num /= 10;
	}
	OLED_ShowString(x, y, str, size);
}

/*---- ˮƽ�������ã������棩----*/
void OLED_ScrollSetup(uint8_t dir, uint8_t start_page, uint8_t end_page, uint8_t speed)
{
	OLED_WR_Byte(0x2E, OLED_CMD);			   // �ȹرչ���
	OLED_WR_Byte(dir ? 0x26 : 0x27, OLED_CMD); // 26��/27��
	OLED_WR_Byte(0x00, OLED_CMD);			   // �����ֽ�
	OLED_WR_Byte(start_page & 0x07, OLED_CMD);
	OLED_WR_Byte((speed & 0x07) << 0, OLED_CMD); // 0-7
	OLED_WR_Byte(end_page & 0x07, OLED_CMD);
	OLED_WR_Byte(0x00, OLED_CMD); // ��ʼ��
	OLED_WR_Byte(0xFF, OLED_CMD); // ������
}

/*---- λͼ���ƣ�������Χ��----*/
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[])
{
	uint32_t idx = 0;
	uint8_t page, col;
	for (page = y0; page <= y1; page++)
	{ // �޸�Ϊ<=
		for (col = x0; col <= x1; col++)
		{ // �޸�Ϊ<=
			if (page < 8 && col < 128)
			{ // ˫�ر���
				OLEDBuffer[page][col] = BMP[idx++];
			}
		}
	}
}