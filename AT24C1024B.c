/*
EEPROM存储芯片
*/
#include "AT24C1024B.h"
#include "Delay.h"
#include <REG52.H>
#include <intrins.h>
#include "stdint.h"
#include <string.h>
sbit SDA = P2 ^ 1; // 虽然和OLED一样都是用了IIC通信，但是为了防止信号串扰，所以这里使用P2.1和P2.0作为IIC信号端口
sbit SCL = P2 ^ 0;

// I2C发送字节
void I2C_SendByte(uint8_t dat)
{   
    uint8_t i;
    uint16_t timeout;
    for (i = 0; i < 8; i++)
    {
        SDA = (dat & 0x80) ? 1 : 0;
        SCL = 1;
        timeout = 1000;
        while (!SCL && timeout--); // 等待时钟线释放
        SCL = 0;
        dat <<= 1;
    }
}

// IIC接受应答
uint8_t I2C_RecvACK()
{
    uint8_t ack;
    SDA = 1;     // 先把SDA拉高
    I2C_Delay(); // 等待SDA被拉高
    SCL = 1;     // 再把SCL拉高
    I2C_Delay(); // 等待SCL被拉高
    ack = SDA;   // 读取SDA
    SCL = 0;     // 把SCL拉低
    I2C_Delay();
    return !ack; // 返回IIC的应答情况
}

// 发送应答
void I2C_SendACK(uint8_t ack)
{
    SDA = ack ? 1 : 0; // 发送ACK或NACK
    SCL = 1;           // 拉高SCL线
    I2C_Delay();       // 等待SCL高电平
    SCL = 0;           // 拉低SCL线，准备下一个位的发送
}
void I2C_Delay()
{
    _nop_();
    _nop_();
}

// 启动IIC
void I2C_Start()
{
    SDA = 1;     // 先把SDA拉高
    I2C_Delay(); // 等待SDA拉高
    SCL = 1;     // 拉高SCL线
    I2C_Delay(); // 等待SCL高电平
    SDA = 0;     // 拉低SDA线
    I2C_Delay(); // 等待SDA拉低
    SCL = 0;     // 拉低SCL线
    I2C_Delay(); // 等待SCL拉低
}

// 结束IIC
void I2C_Stop()
{
    SDA = 0;     // 拉低SDA线
    I2C_Delay(); // 等待SDA拉低
    SCL = 1;     // 拉高SCL线
    I2C_Delay(); // 等待SCL高电平
    SDA = 1;     // 拉高SDA线
    I2C_Delay(); // 等待SDA拉高
}

// IIC接受字节
uint8_t I2C_RecvByte()
{
    uint8_t dat, SDA, i;
    dat = 0;
    SDA = 1;
    for (i = 0; i < 8; i++)
    {                // 接收8位数据
        SCL = 1;     // 先拉高SCL线
        I2C_Delay(); // 等待SCL高电平
        dat <<= 1;   // 左移一位
        dat |= SDA;  // 读取SDA的值
        SCL = 0;     // SCL拉低，准备下一个位的接收
    }
    return dat;
}

// EEPROM读取数据
bool AT24C1024_Read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t devAddrWrite, devAddrRead; // 设备地址和读写地址
    I2C_Start();
    // 发送写命令以设置地址指针
    devAddrWrite = I2C_ADDR | ((addr >> 15) & 0x02); // 根据地址选择设备地址
    I2C_SendByte(devAddrWrite);                      // 向设备地址写入数据
    if (!I2C_RecvACK())
    { // 检测应答情况
        I2C_Stop();
        return false; // 返回失败状态
    }
    // 发送地址高字节
    I2C_SendByte((uint8_t)(addr >> 8));
    if (!I2C_RecvACK())
    {
        I2C_Stop();
        return false; // 返回失败状态
    }
    // 发送地址低字节
    I2C_SendByte((uint8_t)addr);
    if (!I2C_RecvACK())
    {
        I2C_Stop();
        return false; // 返回失败状态
    }
    // 发送重复起始条件并切换到读模式
    I2C_Start();
    devAddrRead = devAddrWrite | 0x01;
    I2C_SendByte(devAddrRead);
    if (!I2C_RecvACK())
    {
        I2C_Stop();
        return false; // 返回失败状态
    }
    // 接收数据
    while (len--)
    { // 读取并接受数据存储到buffer数组中
        *buf++ = I2C_RecvByte();
        // 发送ACK或NACK
        I2C_SendACK(len ? 0 : 1);
    }
    I2C_Stop();
    return true; // 返回成功状态
}

// 向EEPROM存储芯片中写入数据
bool AT24C1024_Write(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t remain, write_len, page_offset; // 剩余数据长度和写入长度
    uint8_t devAddr;                         // 设备地址
    uint16_t i;

    page_offset = addr % PAGE_SIZE;
    while (len > 0) // 循环写入数据
    {
        // 如果剩余数据长度大于等于页大小，则写入页大小数据
        remain = PAGE_SIZE - (addr % PAGE_SIZE);

        // 计算剩余数据长度和写入长度
        remain = (remain == 0) ? PAGE_SIZE : remain;
        write_len = (len > remain) ? remain : len;

        I2C_Start();

        // 设备地址
        devAddr = I2C_ADDR | ((addr >> 15) & 0x02);
        // 向设备地址发送起始信号
        I2C_SendByte(devAddr);
        if (!I2C_RecvACK())
        {
            // 检测设备应答
            I2C_Stop();
            return false; // 返回失败状态
        }
        I2C_SendByte((uint8_t)(addr >> 8));
        if (!I2C_RecvACK())
        {
            I2C_Stop();
            return false; // 返回失败状态
        }
        I2C_SendByte((uint8_t)addr);
        if (!I2C_RecvACK())
        {
            I2C_Stop();
            return false; // 返回失败状态
        }

        // 发送数据并检查ACK
        for (i = 0; i < write_len; i++)
        {
            I2C_SendByte(buf[i]);
            if (!I2C_RecvACK())
            {
                I2C_Stop();
                return false; // 返回失败状态
            }
        }
        I2C_Stop();
        Delay(5);          // 等待写入完成
        addr += write_len; // 更新地址
        buf += write_len;  // 更新数据指针
        len -= write_len;  // 更新当前页剩余长度
    }

    return true; // 返回成功状态
}

// 检查数据有效性
uint8_t CheckDataValid()
{
    uint32_t signature_addr = AT24C1024_SIZE - 4; // 数据的签名地址
    uint8_t sig[4] = {0xAA, 0x55, 0xAA, 0x55};    // 签名数据
    uint8_t read_sig[4];                          // 用于存储读取的数据
    // 从 EEPROM 读取签名数据
    if (!AT24C1024_Read(signature_addr, read_sig, 4))
    {
        return 0; // 读取失败，返回无效
    }
    // 比较签名数据是否匹配
    return memcmp(sig, read_sig, 4) == 0;
}