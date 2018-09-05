/********************************************************************************************************
*date��2018-6-19
*author��NUAA google
*function:clock tree resetting
*input:none
*output:none
*remarks��focus on 8M external XTAL sysclock setted as 168M
********************************************************************************************************/ 
#include "stm32f4xx_rcc.h"
#include "rcc_config.h"

void RCC_Config(void)
{		
	__IO uint32_t HSEStartUpStatus = 0;
	RCC_DeInit();    //RCC�Ĵ�����ʼ��
	RCC_HSEConfig(RCC_HSE_ON);    //ʹ���ⲿʱ��
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if( HSEStartUpStatus== SUCCESS )     //�ȴ��ⲿʱ������
	{
		RCC_PLLCmd(DISABLE);    //����PLLǰӦ�ȹر���PLL
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);    //ѡ��PLLʱ��Ϊϵͳʱ��
		RCC_HCLKConfig(RCC_SYSCLK_Div1);    //HCLK��AHB��ʱ��Ϊϵͳʱ��1��Ƶ
		RCC_PCLK1Config(RCC_HCLK_Div4);    //PCLK��APB1��ʱ��ΪHCLKʱ��8��Ƶ
		RCC_PCLK2Config(RCC_HCLK_Div2);    //PCLK��APB2��ʱ��ΪHCLKʱ��2��Ƶ
		RCC_PLLConfig(RCC_PLLSource_HSE,8,336,2,7);    //PLLʱ�����ã��ⲿ����Ϊ8MHz��ϵͳ����Ϊ168MHz
		RCC_PLLCmd(ENABLE);    //PLLʱ�ӿ���
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);    //�ȴ�PLLʱ��׼����
			}

}