#include "PCF8563.h"
//ȫ�ֱ���,�����ʼ��ֵ����Ҫ��ʼ����ʱ�䣬
//��;��1����ʼ��ʱ�䡣2����ȡ����ʱ��
// �룬�֣�ʱ���죬���ڣ��·ݣ���ݡ�
extern u8 PCF8563_Time[7];
void PCF8563_Init(void)
{
	IIC_Init();
	//ʮ������ת����BCD��
	PCF8563_Time[0] = ((PCF8563_Time[0]/10) << 4) | (PCF8563_Time[0]%10); 
	PCF8563_Time[1] = ((PCF8563_Time[1]/10) << 4) | (PCF8563_Time[1]%10);
	PCF8563_Time[2] = ((PCF8563_Time[2]/10) << 4) | (PCF8563_Time[2]%10); 
	PCF8563_Time[3] = ((PCF8563_Time[3]/10) << 4) | (PCF8563_Time[3]%10); // PCF8563_Time[4] = ((PCF8563_Time[4]/10 << 4)) | (PCF8563_Time[4]%10); �ڲ���ת��
	PCF8563_Time[5] = ((PCF8563_Time[5]/10 << 4)) | (PCF8563_Time[5]%10); 
	PCF8563_Time[6] = ((PCF8563_Time[6]/10 << 4)) | (PCF8563_Time[6]%10); PCF8563_CLKOUT_1s();
	PCF8563_WriteAdress(0x00, 0x20); //��ֹRTC source clock
	//��ʼ��PCF8563��ʱ��
	PCF8563_WriteAdress(0x02, PCF8563_Time[0]);
	PCF8563_WriteAdress(0x03, PCF8563_Time[1]);
	PCF8563_WriteAdress(0x04, PCF8563_Time[2]);
	PCF8563_WriteAdress(0x05, PCF8563_Time[3]);
	PCF8563_WriteAdress(0x06, PCF8563_Time[4]);
	PCF8563_WriteAdress(0x07, PCF8563_Time[5]);
	PCF8563_WriteAdress(0x08, PCF8563_Time[6]);
	PCF8563_WriteAdress(0x00, 0x00); //Enable RTC sorce clock
}
u8 PCF8563_ReaDAdress(u8 Adress)
{
	u8 ReadData;
	IIC_Start();
	IIC_WriteByte(0xa2);
	IIC_WaitAck();
	IIC_WriteByte(Adress);
	IIC_WaitAck();
	IIC_Start();
	IIC_WriteByte(0xa3);
	IIC_WaitAck();
	ReadData = IIC_ReadByte();
	IIC_Stop();
	return ReadData;
}
void PCF8563_WriteAdress(u8 Adress,u8 DataTX) //��
{
	IIC_Start();
	IIC_WriteByte(0xa2);
	IIC_WaitAck();
	IIC_WriteByte(Adress);
	IIC_WaitAck();
	IIC_WriteByte(DataTX);
	IIC_WaitAck();
	IIC_Stop();
}
//ȡ�ذ˸��ֽڵ�����:�룬�֣�ʱ���죬���ڣ��·ݣ���ݡ�
void PCF8563_ReadTimes(void)
{
	IIC_Start();
	IIC_WriteByte(0xa2);
	IIC_WaitAck();
	IIC_WriteByte(0x02);
	IIC_WaitAck();
	IIC_Start();
	IIC_WriteByte(0xa3);
	IIC_WaitAck();
	PCF8563_Time[0] = IIC_ReadByte()&0x7f;
	I2C_Ack();
	PCF8563_Time[1] = IIC_ReadByte()&0x7f;
	I2C_Ack();
	PCF8563_Time[2] = IIC_ReadByte()&0x3f;
	I2C_Ack();
	PCF8563_Time[3] = IIC_ReadByte()&0x3f;
	I2C_Ack();
	PCF8563_Time[4] = IIC_ReadByte()&0x07;
	I2C_Ack();
	PCF8563_Time[5] = IIC_ReadByte()&0x1f;
	I2C_Ack();
	PCF8563_Time[6] = IIC_ReadByte();
	I2C_NoAck();
	IIC_Stop();
	PCF8563_Time[0] = ((PCF8563_Time[0]&0xf0)>>4)*10 + (PCF8563_Time[0]&0x0f); 
	PCF8563_Time[1] = ((PCF8563_Time[1]&0xf0)>>4)*10 + (PCF8563_Time[1]&0x0f);
	PCF8563_Time[2] = ((PCF8563_Time[2]&0xf0)>>4)*10 + (PCF8563_Time[2]&0x0f); 
	PCF8563_Time[3] = ((PCF8563_Time[3]&0xf0)>>4)*10 + (PCF8563_Time[3]&0x0f);
	PCF8563_Time[4] = ((PCF8563_Time[4]&0xf0)>>4)*10 + (PCF8563_Time[4]&0x0f); 
	PCF8563_Time[5] = ((PCF8563_Time[5]&0xf0)>>4)*10 + (PCF8563_Time[5]&0x0f); 
	PCF8563_Time[6] = ((PCF8563_Time[6]&0xf0)>>4)*10 + (PCF8563_Time[6]&0x0f); 
}
//��CLKOUT�϶�ʱ1S���һ���½������壬������֤����������STM32��GPIO�������룬���ó��½����жϣ���Ƭ��ÿ��1S����һ���ж�
void PCF8563_CLKOUT_1s(void)
{
	PCF8563_WriteAdress(0x01, 0); //��ֹ��ʱ��������������
	PCF8563_WriteAdress(0x0e, 0); //�رն�ʱ���ȵ�
	// PCF8563_WriteAdress(0x0e, 0); //д��1
	PCF8563_WriteAdress(0x0d, 0x83); //���������
}