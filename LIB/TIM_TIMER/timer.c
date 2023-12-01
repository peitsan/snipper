#include "timer.h" 
#include "Oled.h"
#include "stdio.h"
Status Light ;                               //���彻ͨ�Ƶ�״??
uint16_t second = 60 ;                       //ʱ�䣬��λ��??
uint16_t foul_people = 0 ;                   //??��Ƶ���??
uint16_t foul_flags  = 1 ;                   //??���??����־λ
uint16_t dat = 0         ;                   //������λ�����͵�����
/****************************TIM2��???��***********************/

void TIME3_init(void)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseStruct.TIM_Prescaler = (72-1);             //Ԥ��ƵΪ72��ÿ��һ����??1us    
  TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up; //���ü�����ʽΪ����???��
	TIM_TimeBaseStruct.TIM_Period = 100;                   //���???��ֵΪ100����100us??����һ����??
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStruct);
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
  TIM_SetCounter(TIMER_TIM,0);
}
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	                                 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //����NCIV????����??2λ��ռ���ȼ�??2λ��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           //����????ͨ��ΪTIM3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //������ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //������Ӧ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);  
	
}

/**********************TIM3����??������*************************/
void TIM3_IRQHandler()
{
	static int i = 0 ; 
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET) //ÿ����һ����????1s
	{
		if(i < Duty)
		{
				GPIO_SetBits(GPIOA,GPIO_Pin_0) ;
		}
		else
		{
			 GPIO_ResetBits(GPIOA,GPIO_Pin_0) ;
		}
	i++ ;
		if(i > 100)
		{
			i = 0 ;
		}
	 
			
	 TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
	}
}

/****************************USART2��???��**************/
//��������ͨ��
void USART2_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE) ;
	//PA2
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                            
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//PA3
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;                     
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate = 115200;                     									     
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;   									        
  USART_InitStruct.USART_StopBits = USART_StopBits_1;        											    
  USART_InitStruct.USART_Parity = USART_Parity_No ;         											   
	 USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  									    
 
	USART_Init(USART2, &USART_InitStruct);
	USART_Cmd(USART2,ENABLE);                               
	
	
	
}

int fputc(int ch,FILE *f)
{
	USART_SendData(USART2, (u8)ch);
	while(!(USART_GetFlagStatus(USART2, USART_FLAG_TXE)==SET));
	return ch;
}

