#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h"                  // Device header
void PWM_Init(void);
void PWM_Set_fan(u8 temp,int set_temp); //设置风扇pwm,temp:当前温度 ， set_temp:设定开启的温度
void PWM_Set_heat(uint16_t Compare); //设置加热电阻pwm，输入1开启，0关闭
#endif


