#ifndef _IIC_H_
#define _IIC_H_
#include "stm32f10x.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*#define I2C_Speed 100000
#define I2C1_SLAVE_ADDRESS7 0xA0
#define I2C_PageSize 256 */
//SCL PA0
//SDA PA1
#define SCL_H() GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define SCL_L() GPIO_ResetBits(GPIOC, GPIO_Pin_7)
#define SDA_H() GPIO_SetBits(GPIOC, GPIO_Pin_8)
#define SDA_L() GPIO_ResetBits(GPIOC, GPIO_Pin_8)
//����0����1
#define Read_SDA() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)
u8 IIC_ReadByte(void);
void IIC_WriteByte(u8 byte);
void IIC_WaitAck(void);
void IIC_Stop(void);
void IIC_Start(void);
void IIC_Init(void);
void I2C_Ack(void);
void I2C_NoAck(void);
//ȡ�ذ˸��ֽڵ�����:�룬�֣�ʱ���죬���ڣ��·ݣ���ݡ�
#endif

