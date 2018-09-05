/*****************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "lcd.h"
#include "pwm.h"
#include "pid_initial.h"
#include "encoder.h"
#include "rcc_config.h"
#include "stdio.h"

/*********ȫ�ֱ�������*********/
extern PidParameters PidMotor_1;

/***����sprintf������ɱ������**/
void LCD_Show(void)
{
char tmp[5];
sprintf(tmp,"%d",PidMotor_1.test_Encoders);
LCD_ShowString(30,200,200,24,24,tmp);
}


/*����������ʼ���񡪡�����*/
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

/*--------���ݽ���-------*/
//�������ȼ�
#define DATEEXCHANGE_TASK_PRIO		2
//�����ջ��С	
#define DATEEXCHANGE_STK_SIZE 		50  
//������
TaskHandle_t DateExchangeTask_Handler;
//������
void DateExchange_task(void *pvParameters);

/*--------PIDʵ��--------*/
//�������ȼ�
#define PIDREALIZE_TASK_PRIO		3
//�����ջ��С	
#define PIDREALIZE_STK_SIZE 		50  
//������
TaskHandle_t PidRealizeTask_Handler;
//������
void PidRealize_task(void *pvParameters);

int main(void)
{ 
	RCC_Config();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	uart_init(115200);     	//��ʼ������
	/************lcd��������*************/
 	LCD_Init();           //��ʼ��LCD FSMC�ӿ�
	POINT_COLOR=BLACK;      //������ɫ����ɫ
	LCD_Clear(WHITE);	
	POINT_COLOR=BLACK;	  
	LCD_ShowString(30,40,280,24,24,"Embeded System ");	
	LCD_ShowString(30,80,280,24,24,"Course Design");	
	LCD_ShowString(30,120,210,24,24,"SX1715007 GuYu");	      					 
	LCD_ShowString(30,160,200,24,24,"2018-9-5");	
	/*************���pid����**************/
	PidMotorsets();//���pid�����趨
	Gpio_Initial();//���GPIO����
	TIM14_PWM_Init(MOTOR1_ARRVALUE - 1,84 - 1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.
	Encoder_Init_TIM2();
	MotorUse(DIR_NEG,ENABLE);
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}
 
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����DateExchange����
    xTaskCreate((TaskFunction_t )DateExchange_task,     	
                (const char*    )"DateExchange_task",   	
                (uint16_t       )DATEEXCHANGE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )DATEEXCHANGE_TASK_PRIO,	
                (TaskHandle_t*  )&DateExchangeTask_Handler);   
    //����PidRealize����
    xTaskCreate((TaskFunction_t )PidRealize_task,     
                (const char*    )"PidRealize_task",   
                (uint16_t       )PIDREALIZE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )PIDREALIZE_TASK_PRIO,
                (TaskHandle_t*  )&PidRealizeTask_Handler);        
								
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//DateExchange������ 
void DateExchange_task(void *pvParameters)
{
    while(1)
    {
			;
    }
}   

//PidRealize������
void PidRealize_task(void *pvParameters)
{
    while(1)
    {
		PidMotor_1.PwmCcrvalue = Pid_PwmContrl();
 		delay_ms(TIME_SAMPLEVALUE);	
		//printf("%d\n",PidMotor_1.test_Encoders);
		LCD_Show();
    }
}
