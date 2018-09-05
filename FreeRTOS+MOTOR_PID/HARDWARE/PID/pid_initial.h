#ifndef _PID_INITIAL_H
#define _PID_INITIAL_H
#include "sys.h"
#include "stdlib.h"
#include "stdio.h"

#define TIME_SAMPLEVALUE 10  //10ms
#define MOTOR1_ARRVALUE 500 //����ֵΪ500

/*****��������*****/
typedef struct Pid
	
{
	float set_velocity;
	float test_velocity;
	int set_Encoders;//����ʱ�趨��ֵ
	int test_Encoders;//����ʱ���õı�����ֵ
	float PwmCcrvalue;
	float Kp;
	float Ki;
	float Kd;
	float PidOut;	
}PidParameters;

/***��������****/
int PidMotorsets(void);
int Pid_SetsGet(void);
float Pid_PwmContrl(void);

#endif