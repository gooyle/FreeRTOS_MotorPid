#include "pwm.h"
#include "led.h"
#include "usart.h"
#include "pid_initial.h"
#include "main.h"
#include "stdint.h"
 
extern PidParameters PidMotor_1;

 
/******************************************
*���pwm���ò��֡���δʹ�ܡ�PF9
*******************************************/
//TIM14 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//TIM14ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOF,&GPIO_InitStructure);              //��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1

	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
 
  TIM_ARRPreloadConfig(TIM14,ENABLE);//ARPEʹ�� 
	
	TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE);//��������ж�
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM14, DISABLE);//ʧ��TIM14
}  

/*******��������巽����������******/
/***************
*INT3����PG6
*INT4����PG7
***************/
void Gpio_Initial(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
	
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOG,GPIO_Pin_6 | GPIO_Pin_7);//int3��int4�˿�����Ϊ0��0 - �ƶ�ģʽ
}

/*********���pwmʹ��*************/
void MotorEnable(void)
{
		TIM_Cmd(TIM14, ENABLE); 								  
}

/*********���pwmʧ��*************/
void MotorDisable(void)
{
		TIM_Cmd(TIM14, DISABLE); 								  
}
/*********��������׽ʹ��********/
void EncoderEnable(void)
{
	TIM_Cmd(TIM2,ENABLE);
}
/**���ʹ�ú���**/
/******************************
*INT3����PG6
*INT4����PG7
*Dir��#DIR_POS ��int3:1��int4:0
*		��#DIR_NEG ��int3:0��int4:1��
*   ��#STOP    ��int3 = int4 = 0;
*******************************/
void MotorUse(uint8_t Dir,uint8_t EnStatement)
{
	if( EnStatement == ENABLE)
	{
		MotorEnable();
		if (Dir == DIR_POS)
		{
			GPIO_SetBits(GPIOG,GPIO_Pin_6);
			GPIO_ResetBits(GPIOG,GPIO_Pin_7);
		}
		else if (Dir == DIR_NEG)
		{
			GPIO_SetBits(GPIOG,GPIO_Pin_7);
			GPIO_ResetBits(GPIOG,GPIO_Pin_6);
		}
		else if (Dir == STOP)
		{
			GPIO_ResetBits(GPIOG,GPIO_Pin_6 | GPIO_Pin_7);
		}
	}
	else //���ʧ��״̬
		MotorDisable();		
}

/*********pwm14�жϷ������*********/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM14,TIM_FLAG_Update) == SET)
	{
		TIM_SetCompare1(TIM14, (uint32_t) PidMotor_1.PwmCcrvalue);	//tim14ͨ��1
	}
 TIM_ClearITPendingBit(TIM14,TIM_IT_Update);//����жϱ�־λ

}