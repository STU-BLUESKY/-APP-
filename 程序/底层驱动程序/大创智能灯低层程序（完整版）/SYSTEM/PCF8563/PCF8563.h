#ifndef _PCF8563_H_
#define _PCF8563_H_
#include "IIC.h"
#define ReadCode 0xa3
#define WriteCode 0xa2
void PCF8563_Init(void);
u8 PCF8563_ReaDAdress(u8 Adress);
void PCF8563_WriteAdress(u8 Adress, u8 DataTX);
//ȡ��7���ֽڵ�����:�룬�֣�ʱ���죬���ڣ��·ݣ���ݡ� //ȫ�ֱ���
extern u8 PCF8563_Time[7];
void PCF8563_ReadTimes(void);
//��CLKOUT�϶�ʱ1S���һ���½�������
void PCF8563_CLKOUT_1s(void);
#endif
