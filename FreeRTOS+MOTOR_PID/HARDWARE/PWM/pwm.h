#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

#define DIR_POS 0
#define DIR_NEG 1
#define STOP 2

void TIM14_PWM_Init(u32 arr,u32 psc);
void Gpio_Initial(void);
void MotorEnable(void);
void MotorDisable(void);
void MotorUse(uint8_t Dir,uint8_t EnStatement);
void TIM8_TRG_COM_TIM14_IRQHandler(void);
void EncoderEnable(void);
#endif
