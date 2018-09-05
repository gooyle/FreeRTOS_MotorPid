#include "pwm.h"
#include "led.h"
#include "usart.h"
#include "pid_initial.h"
#include "main.h"
#include "stdint.h"
 
extern PidParameters PidMotor_1;

 
/******************************************
*电机pwm配置部分——未使能—PF9
*******************************************/
//TIM14 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//TIM14时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure);              //初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
 
  TIM_ARRPreloadConfig(TIM14,ENABLE);//ARPE使能 
	
	TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE);//允许更新中断
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM14, DISABLE);//失能TIM14
}  

/*******电机驱动板方向引脚配置******/
/***************
*INT3——PG6
*INT4——PG7
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
	
	GPIO_ResetBits(GPIOG,GPIO_Pin_6 | GPIO_Pin_7);//int3、int4端口设置为0、0 - 制动模式
}

/*********电机pwm使能*************/
void MotorEnable(void)
{
		TIM_Cmd(TIM14, ENABLE); 								  
}

/*********电机pwm失能*************/
void MotorDisable(void)
{
		TIM_Cmd(TIM14, DISABLE); 								  
}
/*********编码器捕捉使能********/
void EncoderEnable(void)
{
	TIM_Cmd(TIM2,ENABLE);
}
/**电机使用函数**/
/******************************
*INT3——PG6
*INT4——PG7
*Dir—#DIR_POS —int3:1；int4:0
*		—#DIR_NEG —int3:0；int4:1；
*   —#STOP    —int3 = int4 = 0;
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
	else //电机失能状态
		MotorDisable();		
}

/*********pwm14中断服务程序*********/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM14,TIM_FLAG_Update) == SET)
	{
		TIM_SetCompare1(TIM14, (uint32_t) PidMotor_1.PwmCcrvalue);	//tim14通道1
	}
 TIM_ClearITPendingBit(TIM14,TIM_IT_Update);//清除中断标志位

}