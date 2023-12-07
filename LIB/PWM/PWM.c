
#include "pwm.h"
//pwm频率=计数器的更新频率
//占空比=CCR/(ARR+1)
//分辨率=1/(ARR+1)

//PA1:风扇   PA2:加热
void PWM_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;//IO口结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitSturcture;//时基结构体
	TIM_OCInitTypeDef fan_TIM_OCInitStructure;//风扇  输出比较结构体
	TIM_OCInitTypeDef heat_TIM_OCInitStructure;//加热  输出比较结构体
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //开启定时器时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1 | GPIO_Pin_2;  //pwm输出引脚PA1 PA2
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//定时器初始化
	TIM_InternalClockConfig(TIM2);  //使用内部时钟
	
	//初始化时基单元
	TIM_TimeBaseInitSturcture.TIM_ClockDivision=TIM_CKD_DIV1 ; //1分频 不分频
	TIM_TimeBaseInitSturcture.TIM_CounterMode=TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInitSturcture.TIM_Period=100-1;//ARR ,自动重装      定时频率=72M/(PSC+1)/(ARP+1)
	TIM_TimeBaseInitSturcture.TIM_Prescaler=72-1;//PSC，预分频
	TIM_TimeBaseInitSturcture.TIM_RepetitionCounter=0; //高级计数器才有
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitSturcture); 
	
	//风扇初始化输出比较（PA1)
	TIM_OCStructInit(&fan_TIM_OCInitStructure); //给成员赋初始值	
	fan_TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;  //输出模式
	fan_TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High; //高极性，极性不翻转
	fan_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //输出使能
	fan_TIM_OCInitStructure.TIM_Pulse=0;   //CCR	
	TIM_OC2Init(TIM2,&fan_TIM_OCInitStructure);  
	
	
	//加热初始化输出比较(PA2)
	TIM_OCStructInit(&heat_TIM_OCInitStructure); //给成员赋初始值	
	heat_TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;  //输出模式
	heat_TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High; //高极性，极性不翻转
	heat_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //输出使能
	heat_TIM_OCInitStructure.TIM_Pulse=0;   //CCR	
	TIM_OC3Init(TIM2,&heat_TIM_OCInitStructure);  
	

	TIM_Cmd(TIM2,ENABLE);  //启动计数器
}

void PWM_Set_fan(u8 temp, int set_temp)
{
	uint16_t Compare=temp - set_temp + 80;
	TIM_SetCompare2(TIM2,Compare); 
}

void PWM_Set_heat(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare*100);       //更改CCR,当ARR+1=100时，CCR=占空比
}

