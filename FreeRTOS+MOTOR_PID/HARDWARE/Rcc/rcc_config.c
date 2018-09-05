/********************************************************************************************************
*date：2018-6-19
*author：NUAA google
*function:clock tree resetting
*input:none
*output:none
*remarks：focus on 8M external XTAL sysclock setted as 168M
********************************************************************************************************/ 
#include "stm32f4xx_rcc.h"
#include "rcc_config.h"

void RCC_Config(void)
{		
	__IO uint32_t HSEStartUpStatus = 0;
	RCC_DeInit();    //RCC寄存器初始化
	RCC_HSEConfig(RCC_HSE_ON);    //使用外部时钟
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if( HSEStartUpStatus== SUCCESS )     //等待外部时钟启动
	{
		RCC_PLLCmd(DISABLE);    //配置PLL前应先关闭主PLL
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);    //选择PLL时钟为系统时钟
		RCC_HCLKConfig(RCC_SYSCLK_Div1);    //HCLK（AHB）时钟为系统时钟1分频
		RCC_PCLK1Config(RCC_HCLK_Div4);    //PCLK（APB1）时钟为HCLK时钟8分频
		RCC_PCLK2Config(RCC_HCLK_Div2);    //PCLK（APB2）时钟为HCLK时钟2分频
		RCC_PLLConfig(RCC_PLLSource_HSE,8,336,2,7);    //PLL时钟配置，外部晶振为8MHz，系统配置为168MHz
		RCC_PLLCmd(ENABLE);    //PLL时钟开启
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);    //等待PLL时钟准备好
			}

}