#include "oled.h"
#include "oledfont.h"
#include "Delay.h"
#include "music.h"
#include "note.h"
#include "key.h"
// OLED的显存
// 存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127



/**********************************************
//IIC Start I2C发送起始
**********************************************/

void OLED_Reset(void)
{
    OLED_RST_Set();
    Delay(5);  // 短暂延时
    OLED_RST_Clr(); // 复位低电平
    Delay(15); // 保持低电平至少10ms
    OLED_RST_Set(); // 释放复位
    Delay(100);     // 等待复位完成
}

void IIC_Start()
{
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop	I2C发送结束
**********************************************/
void IIC_Stop()
{
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

/**********************************************
//IIC wait	I2C等待接受应答
**********************************************/
void IIC_Wait_Ack()
{
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

/**********************************************
// IIC Write byte	写入字节
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
        OLED_SCLK_Set(); // 上升沿锁存数据
        Delay(2);        // 增加时钟周期延时
        OLED_SCLK_Clr();
        Delay(1);
    }
}

/**********************************************
// IIC Write Command 写命令
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78); // Slave address,SA0=0 从机地址0x78
	IIC_Wait_Ack();
	Write_IIC_Byte(0x00); // write command
	IIC_Wait_Ack();
	Write_IIC_Byte(IIC_Command);
	IIC_Wait_Ack();
	IIC_Stop();
}
/**********************************************
// IIC Write Data 写数据
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
	if (cmd) // 输入0写命令(OLED_CMD) 1写数据(OLED_DATA)
	{
		Write_IIC_Data(dat);
	}
	else
	{
		Write_IIC_Command(dat);
	}
}

/********************************************
// fill_Picture 128位
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

// 坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

// 开启OLED显示
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
	OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}

// 关闭OLED显示
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
	OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}

// 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);	  // 设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);	  // 设置显示位置—列高地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0, OLED_DATA);
	} // 更新显示
}

// 点亮函数 全写为1 整个屏幕点亮
void OLED_On(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);	  // 设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);	  // 设置显示位置—列高地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(1, OLED_DATA);
	} // 更新显示
}

// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反色显示;1,正常显示
// size:选择字体 16/12
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
	unsigned char c = 0, i = 0;
	c = chr - ' '; // 得到偏移后的值
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

// m^n函数
u32 oled_pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

// 显示2个数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// mode:模式	0,填充模式;1,叠加模式
// num:数值(0~4294967295);
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

// 显示一个字符号串
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

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
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

// 初始化SSD1306
void OLED_Init(void)
{
    OLED_Reset(); // 硬件复位
    
    // 增加初始化命令之间的延时
    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
    Delay(20);
    
    OLED_WR_Byte(0xD5, OLED_CMD); // 设置显示时钟分频
    OLED_WR_Byte(0x80, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0xA8, OLED_CMD); // 设置复用比率(1/64)
    OLED_WR_Byte(0x3F, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0x00, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0x40, OLED_CMD); // 设置起始行地址
    Delay(5);
    
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵设置
    OLED_WR_Byte(0x14, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0x20, OLED_CMD); // 内存地址模式
    OLED_WR_Byte(0x00, OLED_CMD); // 水平地址模式
    Delay(5);
    
    OLED_WR_Byte(0xA1, OLED_CMD); // 段重映射(0xA1正常/0xA0镜像)
    OLED_WR_Byte(0xC8, OLED_CMD); // COM扫描方向(0xC8反向/0xC0正常)
    Delay(5);
    
    OLED_WR_Byte(0xDA, OLED_CMD); // COM引脚配置
    OLED_WR_Byte(0x12, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0x81, OLED_CMD); // 对比度设置
    OLED_WR_Byte(0xCF, OLED_CMD); // 亮度值(0x00~0xFF)
    Delay(5);
    
    OLED_WR_Byte(0xD9, OLED_CMD); // 预充电周期
    OLED_WR_Byte(0xF1, OLED_CMD); 
    Delay(5);
    
    OLED_WR_Byte(0xDB, OLED_CMD); // VCOMH电压
    OLED_WR_Byte(0x40, OLED_CMD); // 0x40=1.0*VCC
    Delay(5);
    
    OLED_Clear(); // 初始化后立即清屏
    Delay(100);
    
    OLED_WR_Byte(0xAF, OLED_CMD); // 最后开启显示
    Delay(200); // 确保显示稳定

	OLED_Clear();	// 清屏
}
void OLED_Refresh(void)
{
    OLED_WR_Byte(0x21, OLED_CMD); // 设置列地址范围
    OLED_WR_Byte(0x00, OLED_CMD); // 起始列=0
    OLED_WR_Byte(0x7F, OLED_CMD); // 结束列=127
    OLED_WR_Byte(0x22, OLED_CMD); // 设置页地址范围
    OLED_WR_Byte(0x00, OLED_CMD); // 起始页=0
    OLED_WR_Byte(0x07, OLED_CMD); // 结束页=7
}
// 水平滚动
void OLED_roll(unsigned char track, unsigned char speed)
{
	Write_IIC_Command(0x2D); // 向右(0x2C)/向左(0x2D)滚动一列
	Write_IIC_Command(0x00); // dummy byte(空比特、虚拟字节),暂未发现其指令作用
	if (track == 1)
	{
		Write_IIC_Command(0x00); // 设置滚动起始页地址
		Write_IIC_Command(0x01); // 虚拟字节(设置为0x01)
		Write_IIC_Command(0x07); // 设置滚动结束页
	}
	else
	{
		Write_IIC_Command(0x03);	  // 设置滚动起始页地址
		Write_IIC_Command(speed - 1); // 设置滚动速度(0x00~0x07数值越小速度越慢)
		Write_IIC_Command(0x05);	  // 设置滚动结束页
	}
	Write_IIC_Command(0x00); // 设置起始列地址
	Write_IIC_Command(0x7F); // 设置结束列地址
}