#include "sys.h"
#include "usart.h"	 
#include "24cxx.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���  

u16 RECEIVE_CCR1_VAL;
u16 RECEIVE_CCR2_VAL;
u16 RECEIVE_CCR3_VAL;
u16 RECEIVE_CCR4_VAL;
u16 receive_state=0;
extern u16 power;
extern u8 liangduzidong_flag;
extern u8 xiaoyedeng_flag;
extern u8 close_time_hour;
extern u8 close_time_min;
extern u8 open_time_hour;
extern u8 open_time_min;
extern u8 open_time_change_flag;
extern u8 close_time_change_flag;
extern u8 liangduzidong_flag_change;
extern u8 xiaoyedeng_flag_change;
extern u8 yanseziduojianbian_flag;
extern u8 yanseziduojianbian_flag_change;

struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������			
		switch(receive_state)
		{
			case 0:
				if(Res==0x0d)//����ͷ
				{
					receive_state++;
				}
				else if(Res==0x03)
				{
					receive_state=5;//�����Զ�
				}
				else if(Res==0x02)
				{
					receive_state=6;//Сҹ��
				}
				else if(Res==0xaa)//170
				{
					receive_state=7;//��ʱ����
				}
				else if(Res==0xcc)//204
				{
					receive_state=9;//��ʱ�ص�
				}
				else if(Res==0x04)
				{
					receive_state=11;//��ɫ����
				}
				break;
			case 1:
				RECEIVE_CCR1_VAL=Res;
				receive_state++;
				break;
			case 2:
				RECEIVE_CCR2_VAL=Res;
				receive_state++;
				break;
			case 3:
				RECEIVE_CCR3_VAL=Res;
				receive_state++;
				break;
			case 4:
				power=Res;
				receive_state=12;
				break;
			case 5:           //�����Զ�
				liangduzidong_flag_change=1;
				liangduzidong_flag=Res;				
				receive_state=12;
				break;
			case 6:          //Сҹ��
				xiaoyedeng_flag_change=1;
				xiaoyedeng_flag=Res;				
				receive_state=12;
				break;
			case 7:        //��ʱ����		
				open_time_hour=Res;				
				receive_state=8;
				break;
			case 8:
				open_time_change_flag=1;
				open_time_min=Res;				
				receive_state=12;
				break;
			case 9:        //��ʱ�ص�
				close_time_hour=Res;
				receive_state=10;
				break;
			case 10:
				close_time_change_flag=1;
				close_time_min=Res;			
				receive_state=12;
				break;
			case 11:
				yanseziduojianbian_flag_change=1;
				yanseziduojianbian_flag=Res;
				receive_state=12;
				break;
			case 12:
				if(Res==0x0a)//����β
				{
					receive_state=0;
				}
				break;
		}
	
	} 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	

