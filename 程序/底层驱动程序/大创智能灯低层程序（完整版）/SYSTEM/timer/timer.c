#include "timer.h"
#include "public.h"
#include "PCF8563.h"
#include "24cxx.h"

extern u16 led0pwmval;    
extern u8 dir_1;
extern u8 dir_2;
extern u8 dir_3;
extern u8 dir_4;
extern u16 CCR1_VAL;//500
extern u16 CCR2_VAL;//375
extern u16 CCR3_VAL;//250
extern u16 CCR4_VAL;//125
extern u16 RECEIVE_CCR1_VAL;
extern u16 RECEIVE_CCR2_VAL;
extern u16 RECEIVE_CCR3_VAL;
extern u16 RECEIVE_CCR4_VAL;
extern u16 power;
extern u8 liangduzidong_flag;
extern u8 PCF8563_Time[7];
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

//HSL
extern double CCR_H;
extern double CCR_S;
extern double CCR_L;
//RBG
extern double CCR_R;
extern double CCR_B;
extern double CCR_G;


/*
*��ʱ��3��4·PWM��� PB1����
*/
void TIM3_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
}

void TIM3_Mode_Config()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;	
	
	TIM_TimeBaseStructure.TIM_Period=255;//����Ϊ255
	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=CCR1_VAL;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=CCR2_VAL;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=CCR3_VAL;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);	
	
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=CCR4_VAL;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);	
	
	TIM_CtrlPWMOutputs(TIM3,ENABLE); 
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
		
}
/*
*��ʱ��2��ʱ��Ƭ ���������һ��
*/
void Timer2_Cfg(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_DeInit(TIM2);
	TIM_InternalClockConfig(TIM2);//ѡ���ڲ�ʱ��
	TIM_TimeBaseStructure.TIM_Period=2000;//�Զ���װ�ؼĴ�����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler=71;//ʱ��Ԥ����������Ƶ��Ϊ2kmz
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);//�������жϱ�־
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//������ʱ���жϣ���NVIC�������൱��˫���գ������Ժ�����رն�ʱ��
	
	TIM_Cmd(TIM2,ENABLE);
	
//	TIM_PrescalerConfig(TIM2,35999,TIM_PSCReloadMode_Immediate);//�ö�ʱ��Ԥ��Ƶ����װ��
	
	//�ж�����
	//NVIC_PriorityGoupConfig(NVIC_PriorityGroup_2);//ѡ���������ȼ���
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;//ѡ���ж�����ͨ��Ϊ��ʱ��2��ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�����
	NVIC_Init(&NVIC_InitStructure);
	
}

/*
*��ʱ��2������ 
*�ı�Ƶ�PWMֵ
*/
void TIM2_IRQHandler()
{
	int Signal;
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);	
	Signal = ADC_GetConversionValue(ADC1)/4/4;
	PCF8563_ReadTimes();
	if(open_time_change_flag)
	{
		AT24CXX_WriteOneByte(0,open_time_hour);
		AT24CXX_WriteOneByte(1,open_time_min);
		open_time_change_flag=0;
	}
	if(close_time_change_flag)
	{
		AT24CXX_WriteOneByte(2,close_time_hour);
		AT24CXX_WriteOneByte(3,close_time_min);
		close_time_change_flag=0;
	}
	if(liangduzidong_flag_change)
	{
		AT24CXX_WriteOneByte(4,liangduzidong_flag);
		liangduzidong_flag_change=0;
	}
	if(xiaoyedeng_flag_change)
	{
		AT24CXX_WriteOneByte(5,xiaoyedeng_flag);
		xiaoyedeng_flag_change=0;
	}
	if(yanseziduojianbian_flag_change)
	{
		AT24CXX_WriteOneByte(6,yanseziduojianbian_flag);
		yanseziduojianbian_flag_change=0;
	}
	if(open_time_hour==PCF8563_Time[2]&&open_time_min==PCF8563_Time[1]&&PCF8563_Time[0]==58)
	{
		power=1;
	}
	if(close_time_hour==PCF8563_Time[2]&&close_time_min==PCF8563_Time[1]&&PCF8563_Time[0]==58)
	{
		power=0;
	}
	if(power==0)
	{
		RECEIVE_CCR1_VAL=0;
		RECEIVE_CCR2_VAL=0;
		RECEIVE_CCR3_VAL=0;
		//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE);
	}
	if(power==1)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
		TIM3_Mode_Config();
	}
	if(liangduzidong_flag==1)
	{
		RECEIVE_CCR1_VAL=Signal;
		RECEIVE_CCR2_VAL=Signal;
		RECEIVE_CCR3_VAL=Signal;
		CCR1_VAL=Signal;
		CCR2_VAL=Signal;
		CCR3_VAL=Signal;
	}
	if(xiaoyedeng_flag==1)
	{
		RECEIVE_CCR1_VAL=125;
		RECEIVE_CCR2_VAL=44;
		RECEIVE_CCR3_VAL=200;
	}
	if(yanseziduojianbian_flag==1)//��ɫ�Զ�����
	{
		RGB2HSL();
		CCR_H+=0.00003;
		HSL2RGB(CCR_H,CCR_S,CCR_L);
		TIM_SetCompare1(TIM3,CCR_R);
		TIM_SetCompare2(TIM3,CCR_B);
		TIM_SetCompare3(TIM3,CCR_G);
	}
	if(CCR1_VAL!=RECEIVE_CCR1_VAL)
	{
		if(CCR1_VAL>RECEIVE_CCR1_VAL)
		{
			CCR1_VAL--;
			TIM_SetCompare1(TIM3,CCR1_VAL);
			return;
		}
		if(CCR1_VAL<RECEIVE_CCR1_VAL)
		{
			CCR1_VAL++;
			TIM_SetCompare1(TIM3,CCR1_VAL);
			return;
		}
	}
	if(CCR2_VAL!=RECEIVE_CCR2_VAL)
	{
		if(CCR2_VAL>RECEIVE_CCR2_VAL)
		{
			CCR2_VAL--;
			TIM_SetCompare2(TIM3,CCR2_VAL);
			return;
		}
		if(CCR2_VAL<RECEIVE_CCR2_VAL)
		{
			CCR2_VAL++;
			TIM_SetCompare2(TIM3,CCR2_VAL);
			return;
		}
	}
	if(CCR3_VAL!=RECEIVE_CCR3_VAL)
	{
		if(CCR3_VAL>RECEIVE_CCR3_VAL)
		{
			CCR3_VAL--;
			TIM_SetCompare3(TIM3,CCR3_VAL);
			return;
		}
		if(CCR3_VAL<RECEIVE_CCR3_VAL)
		{
			CCR3_VAL++;
			TIM_SetCompare3(TIM3,CCR3_VAL);
			return;
		}
	}	   
}

