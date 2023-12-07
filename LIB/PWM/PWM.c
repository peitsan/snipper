
#include "pwm.h"
//pwmƵ��=�������ĸ���Ƶ��
//ռ�ձ�=CCR/(ARR+1)
//�ֱ���=1/(ARR+1)

//PA1:����   PA2:����
void PWM_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;//IO�ڽṹ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitSturcture;//ʱ���ṹ��
	TIM_OCInitTypeDef fan_TIM_OCInitStructure;//����  ����ȽϽṹ��
	TIM_OCInitTypeDef heat_TIM_OCInitStructure;//����  ����ȽϽṹ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //������ʱ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;  //�������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1 | GPIO_Pin_2;  //pwm�������PA1 PA2
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//��ʱ����ʼ��
	TIM_InternalClockConfig(TIM2);  //ʹ���ڲ�ʱ��
	
	//��ʼ��ʱ����Ԫ
	TIM_TimeBaseInitSturcture.TIM_ClockDivision=TIM_CKD_DIV1 ; //1��Ƶ ����Ƶ
	TIM_TimeBaseInitSturcture.TIM_CounterMode=TIM_CounterMode_Up;  //���ϼ���ģʽ
	TIM_TimeBaseInitSturcture.TIM_Period=100-1;//ARR ,�Զ���װ      ��ʱƵ��=72M/(PSC+1)/(ARP+1)
	TIM_TimeBaseInitSturcture.TIM_Prescaler=72-1;//PSC��Ԥ��Ƶ
	TIM_TimeBaseInitSturcture.TIM_RepetitionCounter=0; //�߼�����������
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitSturcture); 
	
	//���ȳ�ʼ������Ƚϣ�PA1)
	TIM_OCStructInit(&fan_TIM_OCInitStructure); //����Ա����ʼֵ	
	fan_TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;  //���ģʽ
	fan_TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High; //�߼��ԣ����Բ���ת
	fan_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //���ʹ��
	fan_TIM_OCInitStructure.TIM_Pulse=0;   //CCR	
	TIM_OC2Init(TIM2,&fan_TIM_OCInitStructure);  
	
	
	//���ȳ�ʼ������Ƚ�(PA2)
	TIM_OCStructInit(&heat_TIM_OCInitStructure); //����Ա����ʼֵ	
	heat_TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;  //���ģʽ
	heat_TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High; //�߼��ԣ����Բ���ת
	heat_TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //���ʹ��
	heat_TIM_OCInitStructure.TIM_Pulse=0;   //CCR	
	TIM_OC3Init(TIM2,&heat_TIM_OCInitStructure);  
	

	TIM_Cmd(TIM2,ENABLE);  //����������
}

void PWM_Set_fan(u8 temp, int set_temp)
{
	uint16_t Compare=temp - set_temp + 80;
	TIM_SetCompare2(TIM2,Compare); 
}

void PWM_Set_heat(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare*100);       //����CCR,��ARR+1=100ʱ��CCR=ռ�ձ�
}

