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

/*********全局变量声明*********/
extern PidParameters PidMotor_1;

/***调用sprintf函数完成变量输出**/
void LCD_Show(void)
{
char tmp[5];
sprintf(tmp,"%d",PidMotor_1.test_Encoders);
LCD_ShowString(30,200,200,24,24,tmp);
}


/*————开始任务————*/
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

/*--------数据交互-------*/
//任务优先级
#define DATEEXCHANGE_TASK_PRIO		2
//任务堆栈大小	
#define DATEEXCHANGE_STK_SIZE 		50  
//任务句柄
TaskHandle_t DateExchangeTask_Handler;
//任务函数
void DateExchange_task(void *pvParameters);

/*--------PID实现--------*/
//任务优先级
#define PIDREALIZE_TASK_PRIO		3
//任务堆栈大小	
#define PIDREALIZE_STK_SIZE 		50  
//任务句柄
TaskHandle_t PidRealizeTask_Handler;
//任务函数
void PidRealize_task(void *pvParameters);

int main(void)
{ 
	RCC_Config();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	/************lcd参数设置*************/
 	LCD_Init();           //初始化LCD FSMC接口
	POINT_COLOR=BLACK;      //画笔颜色：红色
	LCD_Clear(WHITE);	
	POINT_COLOR=BLACK;	  
	LCD_ShowString(30,40,280,24,24,"Embeded System ");	
	LCD_ShowString(30,80,280,24,24,"Course Design");	
	LCD_ShowString(30,120,210,24,24,"SX1715007 GuYu");	      					 
	LCD_ShowString(30,160,200,24,24,"2018-9-5");	
	/*************电机pid调试**************/
	PidMotorsets();//电机pid参数设定
	Gpio_Initial();//电机GPIO配置
	TIM14_PWM_Init(MOTOR1_ARRVALUE - 1,84 - 1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.
	Encoder_Init_TIM2();
	MotorUse(DIR_NEG,ENABLE);
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}
 
//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建DateExchange任务
    xTaskCreate((TaskFunction_t )DateExchange_task,     	
                (const char*    )"DateExchange_task",   	
                (uint16_t       )DATEEXCHANGE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )DATEEXCHANGE_TASK_PRIO,	
                (TaskHandle_t*  )&DateExchangeTask_Handler);   
    //创建PidRealize任务
    xTaskCreate((TaskFunction_t )PidRealize_task,     
                (const char*    )"PidRealize_task",   
                (uint16_t       )PIDREALIZE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )PIDREALIZE_TASK_PRIO,
                (TaskHandle_t*  )&PidRealizeTask_Handler);        
								
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//DateExchange任务函数 
void DateExchange_task(void *pvParameters)
{
    while(1)
    {
			;
    }
}   

//PidRealize任务函数
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
