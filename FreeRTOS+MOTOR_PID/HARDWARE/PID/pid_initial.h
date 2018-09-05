#ifndef _PID_INITIAL_H
#define _PID_INITIAL_H
#include "sys.h"
#include "stdlib.h"
#include "stdio.h"

#define TIME_SAMPLEVALUE 10  //10ms
#define MOTOR1_ARRVALUE 500 //满载值为500

/*****变量定义*****/
typedef struct Pid
	
{
	float set_velocity;
	float test_velocity;
	int set_Encoders;//调试时设定此值
	int test_Encoders;//调试时所得的编码数值
	float PwmCcrvalue;
	float Kp;
	float Ki;
	float Kd;
	float PidOut;	
}PidParameters;

/***函数声明****/
int PidMotorsets(void);
int Pid_SetsGet(void);
float Pid_PwmContrl(void);

#endif