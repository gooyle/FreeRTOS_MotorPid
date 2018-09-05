#include "pid_initial.h"
#include "encoder.h"
#include "main.h"
#include "math.h"
#include "delay.h"

PidParameters PidMotor_1;

/*********��ʼ��pid��������************/
int PidMotorsets(void)
{
	PidMotor_1.Kp = 56;
	PidMotor_1.Ki = 0.12;
	//PidMotor_1.set_velocity ; 
	PidMotor_1.set_Encoders = 50;
	PidMotor_1.PwmCcrvalue = 0;
	return 0;
    }

/**************************************************
*��ֵ�������趨ѭ��ֵ�Ա�֤��ͬѭ��ʱ����CNTֵ���ֲ���
*���룺����ʱ��Time_MotorSample
*�����Encoder_DifferValue
******************************************/
int Pid_SetsGet(void)
{
	int Encoder_DifferValue = 0;

	Encoder_DifferValue =  abs((uint16_t)(TIM_GetCounter(TIM2)) - Encoder_InitialValue);//����Ǹ��µı���
	TIM_SetCounter(TIM2,Encoder_InitialValue);//δ��ֹ�������������CNT

	//delay_ms(TIME_SAMPLEVALUE);
	//printf("%d\n",Encoder_DifferValue);
	return Encoder_DifferValue;
}

/**************************************************
*���룺��������Time_MotorSample
*�����PWM����CCR����ֵ
*���ж��е��ô˺���
*************************************/
float Pid_PwmContrl(void)
{
	int CcrValue ;
	static float Et = 0.0;
	static float Sum_Et = 0.0;//Et:�趨ֵ��ʵ��ֵ�Ĳ��ӦKp
												//Sum_Et:��ɢ���������в�ֵ������ͣ���ӦKi
	int Encoder_DifferValue = 0;

	PidMotor_1.test_Encoders = Pid_SetsGet();//����ֱ�ӽ������ı�������ֵ����	
	Et = PidMotor_1.set_Encoders - PidMotor_1.test_Encoders;
	Sum_Et = Sum_Et + Et;
	CcrValue = (PidMotor_1.Kp * Et + PidMotor_1.Ki * Sum_Et);
	//printf("%f\n",Et);
	if(CcrValue >= MOTOR1_ARRVALUE)
		CcrValue = MOTOR1_ARRVALUE ;
	else if(CcrValue <= 0)
		CcrValue = 0;
	else if((CcrValue > 0) && (CcrValue < MOTOR1_ARRVALUE))
		CcrValue = CcrValue;
	return CcrValue;
}
