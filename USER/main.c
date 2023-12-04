#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "dth11.h"
#include "stdio.h"
#include "Oled.h"
//ͷ�ļ�����

// #define key_set PBin(11)//���ð���
// #define key_plus PBin(12)//��ʪ��+
// #define key_sub PBin(13)//��ʪ��-
#define key_set PBin(10)//���ð���
#define key_plus PBin(1)//��ʪ��+
#define key_sub PBin(0)//��ʪ��-
// #define motor_hum PBout(10)//��ʪ���
// #define motor_fan PBout(14)//ɢ�ȷ���
// #define beep PBout(15)//������
#define motor_hum PAout(2)//��ʪ���
#define motor_fan PAout(1)//ɢ�ȷ���
#define beep PBout(12)//������
//�궨��

void OUTPUT_Init(void)
{ 
 GPIO_InitTypeDef  GPIOA_InitStructure, GPIOB_InitStructure;
 	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��P�˿�ʱ��
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��P�˿�ʱ��	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_10;				 //IO����˿�����
 GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;				 //IO����˿�����
 GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_12| IO_DHT11;				 //IO����˿�����
 GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIOA_InitStructure);
 GPIO_Init(GPIOB, &GPIOB_InitStructure);	

 GPIO_SetBits(GPIOA,GPIO_Pin_2);						 //�����
 GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //�����
 GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //�����
}

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
int main(void)
{	 
	int buf1=0,buf2=30,buf3=30;
////////////////////////////////////////////////////////////////////////////////// 	 
// ����˵����
// buf1:������������  0:�¶�  1:ʪ��
// buf2:�趨�¶�ֵ   0.0~100.0���϶�
// buf3:�趨ʪ��ֵ   0.0%~100.0%
////////////////////////////////////////////////////////////////////////////////// 	 

	u8 temp = 0, hum = 0;
	char buf[100] = { 0 };
	DELAY_Init();//���涼�ǳ�ʼ������
	OLED_Init() ; 	
	DHT11_Init();
	KEY_Init();
	OUTPUT_Init();
	while(1)
	{
		DHT11_Read_Data(&temp, &hum);//��ʪ�ȶ�ȡ
		// printf("temp:%d hum:%d", temp, hum);//���ڴ�ӡ
		sprintf((char *)buf, "Temp:%d C", buf);//����ת�ַ��� �¶�
		OLED_ShowString(1, 1, buf);//�ַ�����ʾ
		sprintf((char *)buf, "Humi:%d " , buf);//����ת�ַ���   ʪ��
		OLED_ShowString(3, 1, buf);//�ַ�����ʾ

		if(key_set==1) //���ð���
		{	
			while(key_set!=0);
				buf1++;             //����
			if(buf1>1) buf1=0;    //�� 1/�� 0
		}
		if(key_plus==1 && buf1==0) //�����¶�ֵ+1
		{		
			while(key_plus!=0); 
				buf2++;	
			if(buf2>100) buf2=100,OLED_Init();			
		}
		if(key_sub==1&&buf1==0) //�����¶�ֵ-1
		{		
			while(key_sub!=0);
				buf2--;	
			if(buf2<1) buf2=1,OLED_Init();			
		}
		
		if(key_plus==1&&buf1==1) //����ʪ��ֵ+1
		{		
			while(key_plus!=0);
				buf3++;		
			if(buf3>100) buf3=100,OLED_Init();			
		}

		if(key_sub==1&&buf1==1) //����ʪ��ֵ-1
		{		
			while(key_sub!=0);
				buf3--;		
			if(buf3<1) buf3=1,OLED_Init();			
		}
		sprintf((char *)buf, "Temp Set:%d ", buf2);//����ת�ַ��� �¶�
		OLED_ShowString(2, 1, buf);//�ַ�����ʾ
		sprintf((char *)buf, "Humi Set:%d ", buf3);//����ת�ַ���   ʪ��
		OLED_ShowString(4, 1, buf);//�ַ�����ʾ

			
		if(temp>buf2||hum<buf3)  beep=0;//�¶ȹ��߻���ʪ�ȹ��� ��������������
		else beep=1;
		if(temp>buf2) motor_fan=0;//�¶ȴ�������ֵ  ����ɢ��
		else motor_fan=1;
		if(hum<buf3) motor_hum=0;//ʪ�ȹ���  ������ʪ
		else motor_hum=1;
		if(buf1==0)//��ʾ����״̬
		{
			sprintf((char *)buf, "Temp Set:%d C", 1);//����ת�ַ��� �¶�
			OLED_ShowString(2, 1, buf);//�ַ�����ʾ
		}
		else //��ʾ����״̬
		{
			sprintf((char *)buf, "Humi Set:%d ", 1);//����ת�ַ��� ʪ��
			OLED_ShowString(4, 1, buf);//�ַ�����ʾ
		}
		
	}
}


