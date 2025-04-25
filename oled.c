#include "oled.h"
#include "oledfont.h"
#include "Delay.h"
#include "music.h"
#include "note.h"
#include "key.h"
// OLED���Դ�
// ��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127



/**********************************************
//IIC Start I2C������ʼ
**********************************************/

void OLED_Reset(void)
{
    OLED_RST_Set();
    Delay(5);  // ������ʱ
    OLED_RST_Clr(); // ��λ�͵�ƽ
    Delay(15); // ���ֵ͵�ƽ����10ms
    OLED_RST_Set(); // �ͷŸ�λ
    Delay(100);     // �ȴ���λ���
}

void IIC_Start()
{
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop	I2C���ͽ���
**********************************************/
void IIC_Stop()
{
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

/**********************************************
//IIC wait	I2C�ȴ�����Ӧ��
**********************************************/
void IIC_Wait_Ack()
{
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

/**********************************************
// IIC Write byte	д���ֽ�
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    for(i=0;i<8;i++)
    { 
        OLED_SCLK_Clr();
        if(IIC_Byte & 0x80)
            OLED_SDIN_Set();
        else
            OLED_SDIN_Clr();
        IIC_Byte <<= 1;
        OLED_SCLK_Set(); // ��������������
        Delay(2);        // ����ʱ��������ʱ
        OLED_SCLK_Clr();
        Delay(1);
    }
}

/**********************************************
// IIC Write Command д����
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78); // Slave address,SA0=0 �ӻ���ַ0x78
	IIC_Wait_Ack();
	Write_IIC_Byte(0x00); // write command
	IIC_Wait_Ack();
	Write_IIC_Byte(IIC_Command);
	IIC_Wait_Ack();
	IIC_Stop();
}
/**********************************************
// IIC Write Data д����
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78); // D/C#=0; R/W#=0
	IIC_Wait_Ack();
	Write_IIC_Byte(0x40); // write data
	IIC_Wait_Ack();
	Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();
	IIC_Stop();
}
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
	if (cmd) // ����0д����(OLED_CMD) 1д����(OLED_DATA)
	{
		Write_IIC_Data(dat);
	}
	else
	{
		Write_IIC_Command(dat);
	}
}

/********************************************
// fill_Picture 128λ
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m, n;
	for (m = 0; m < 8; m++)
	{
		OLED_WR_Byte(0xb0 + m, 0); // page0-page1
		OLED_WR_Byte(0x00, 0);	   // low column start address
		OLED_WR_Byte(0x10, 0);	   // high column start address
		for (n = 0; n < 128; n++)
		{
			OLED_WR_Byte(fill_Data, 1);
		}
	}
}

// ��������
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

// ����OLED��ʾ
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC����
	OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}

// �ر�OLED��ʾ
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC����
	OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}

// ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // ����ҳ��ַ��0~7��
		OLED_WR_Byte(0x00, OLED_CMD);	  // ������ʾλ�á��е͵�ַ
		OLED_WR_Byte(0x10, OLED_CMD);	  // ������ʾλ�á��иߵ�ַ
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0, OLED_DATA);
	} // ������ʾ
}

// �������� ȫдΪ1 ������Ļ����
void OLED_On(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // ����ҳ��ַ��0~7��
		OLED_WR_Byte(0x00, OLED_CMD);	  // ������ʾλ�á��е͵�ַ
		OLED_WR_Byte(0x10, OLED_CMD);	  // ������ʾλ�á��иߵ�ַ
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(1, OLED_DATA);
	} // ������ʾ
}

// ��ָ��λ����ʾһ���ַ�,���������ַ�
// x:0~127
// y:0~63
// mode:0,��ɫ��ʾ;1,������ʾ
// size:ѡ������ 16/12
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
	unsigned char c = 0, i = 0;
	c = chr - ' '; // �õ�ƫ�ƺ��ֵ
	if (x > Max_Column - 1)
	{
		x = 0;
		y = y + 2;
	}
	if (Char_Size == 16)
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
	}
	else
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 6; i++)
			OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}

// m^n����
u32 oled_pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

// ��ʾ2������
// x,y :�������
// len :���ֵ�λ��
// size:�����С
// mode:ģʽ	0,���ģʽ;1,����ģʽ
// num:��ֵ(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
	}
}

// ��ʾһ���ַ��Ŵ�
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WR_Byte(BMP[j++], OLED_DATA);
		}
	}
}

// ��ʼ��SSD1306
void OLED_Init(void)
{
    OLED_Reset(); // Ӳ����λ
    
    // ���ӳ�ʼ������֮�����ʱ
    OLED_WR_Byte(0xAE, OLED_CMD); // �ر���ʾ
    Delay(20);
    
    OLED_WR_Byte(0xD5, OLED_CMD); // ������ʾʱ�ӷ�Ƶ
    OLED_WR_Byte(0x80, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0xA8, OLED_CMD); // ���ø��ñ���(1/64)
    OLED_WR_Byte(0x3F, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0xD3, OLED_CMD); // ������ʾƫ��
    OLED_WR_Byte(0x00, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0x40, OLED_CMD); // ������ʼ�е�ַ
    Delay(5);
    
    OLED_WR_Byte(0x8D, OLED_CMD); // ��ɱ�����
    OLED_WR_Byte(0x14, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0x20, OLED_CMD); // �ڴ��ַģʽ
    OLED_WR_Byte(0x00, OLED_CMD); // ˮƽ��ַģʽ
    Delay(5);
    
    OLED_WR_Byte(0xA1, OLED_CMD); // ����ӳ��(0xA1����/0xA0����)
    OLED_WR_Byte(0xC8, OLED_CMD); // COMɨ�跽��(0xC8����/0xC0����)
    Delay(5);
    
    OLED_WR_Byte(0xDA, OLED_CMD); // COM��������
    OLED_WR_Byte(0x12, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0x81, OLED_CMD); // �Աȶ�����
    OLED_WR_Byte(0xCF, OLED_CMD); // ����ֵ(0x00~0xFF)
    Delay(5);
    
    OLED_WR_Byte(0xD9, OLED_CMD); // Ԥ�������
    OLED_WR_Byte(0xF1, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0xDB, OLED_CMD); // VCOMH��ѹ
    OLED_WR_Byte(0x40, OLED_CMD); // 0x40=1.0*VCC
    Delay(5);
    
    OLED_Clear(); // ��ʼ������������
    Delay(100);
    
    OLED_WR_Byte(0xAF, OLED_CMD); // �������ʾ
    Delay(200); // ȷ����ʾ�ȶ�

	OLED_Clear();	// ����
}
void OLED_Refresh(void)
{
    OLED_WR_Byte(0x21, OLED_CMD); // �����е�ַ��Χ
    OLED_WR_Byte(0x00, OLED_CMD); // ��ʼ��=0
    OLED_WR_Byte(0x7F, OLED_CMD); // ������=127
    OLED_WR_Byte(0x22, OLED_CMD); // ����ҳ��ַ��Χ
    OLED_WR_Byte(0x00, OLED_CMD); // ��ʼҳ=0
    OLED_WR_Byte(0x07, OLED_CMD); // ����ҳ=7
}
// ˮƽ����
void OLED_roll(unsigned char track, unsigned char speed)
{
	Write_IIC_Command(0x2D); // ����(0x2C)/����(0x2D)����һ��
	Write_IIC_Command(0x00); // dummy byte(�ձ��ء������ֽ�),��δ������ָ������
	if (track == 1)
	{
		Write_IIC_Command(0x00); // ���ù�����ʼҳ��ַ
		Write_IIC_Command(0x01); // �����ֽ�(����Ϊ0x01)
		Write_IIC_Command(0x07); // ���ù�������ҳ
	}
	else
	{
		Write_IIC_Command(0x03);	  // ���ù�����ʼҳ��ַ
		Write_IIC_Command(speed - 1); // ���ù����ٶ�(0x00~0x07��ֵԽС�ٶ�Խ��)
		Write_IIC_Command(0x05);	  // ���ù�������ҳ
	}
	Write_IIC_Command(0x00); // ������ʼ�е�ַ
	Write_IIC_Command(0x7F); // ���ý����е�ַ
}