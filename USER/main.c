#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stdio.h"
#include "Oled.h"
#include "pwm.h"
//ͷ�ļ�����

#define key_set PBin(10)//���ð���
#define key_plus PBin(1)//��ʪ��+
#define key_sub PBin(0)//��ʪ��-
#define motor_fan PAout(1)//ɢ�ȷ���
#define motor_hum PBout(13)//��ʪ���
#define beep PBout(12)//������
#define long_press_time 500 //����ʱ��
#define key_speed 50 //˫��ʱ��
#define key_status_once 1//����״̬
#define key_status_double 2 //˫��״̬
#define key_status_long 3 //����״̬

//�궨��



void OUTPUT_Init(void)
{ 
 GPIO_InitTypeDef  GPIOA_InitStructure, GPIOB_InitStructure;
 	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��P�˿�ʱ��
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��P�˿�ʱ��	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_10;				 //IO����˿�����
 GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;				 //IO����˿�����
 GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13 | DHT11_Out_Pin;				 //IO����˿�����
 GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIOA_InitStructure);
 GPIO_Init(GPIOB, &GPIOB_InitStructure);	

 GPIO_SetBits(GPIOA,GPIO_Pin_2);						 //�����
 GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //�����
 GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //�����
 GPIO_SetBits(GPIOB,GPIO_Pin_13);
}

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u8 key_setting(void)
{
    u8 b, c;
    if(key_set == 0) //���ð�������
    {
        delay_ms(20); //��ʱȥ����
        if(key_set == 0) //���ð�������
        {
            c = 0;
            while(key_set == 0) //���������£���ʼ����
            {
                c++;
                delay_ms(10);
                if(c >= long_press_time) //����
                {
                    return key_status_long;
                }
            }
            //�������ͷ�
            for(b = 0; b < key_speed; b++)
            {
                delay_ms(20);
                if(key_set == 0) //�ڶ��ΰ�������
                {
                    delay_ms(10);
                    if(key_set == 0) //�ڶ��ΰ�����������
                    {
                        while(key_set == 0); //�ȴ������ͷ�
                        return key_status_double; //˫��
                    }
                }
            }
            //û�еڶ��ΰ������������ص���
            return key_status_once;
        }
    }
    //����û�б����£�����0
    return 0;
}

int main(void)
{	 
		 
	u8 temp = 0, hum = 0, key_status;
	int buf1=0,buf2=30,buf3=70;
////////////////////////////////////////////////////////////////////////////////// 	 
// ����˵����
// buf1:������������ 0������ 1:�¶�  2:ʪ��
// buf2:�趨�¶�ֵ   0.0~100.0���϶�
// buf3:�趨ʪ��ֵ   0.0%~100.0%
////////////////////////////////////////////////////////////////////////////////// 
	DHT11_Data_TypeDef data;
	unsigned char buf[127] = { 0 };
	DELAY_Init();//���涼�ǳ�ʼ������
	OLED_Init() ; 
	PWM_Init();	
	KEY_Init();
	OUTPUT_Init();
	while(1)
	{
		Read_DHT11(&data);
		sprintf((char *)buf, "Temp:%d.%d C", data.temp_int, data.temp_deci);//����ת�ַ��� �¶�
		OLED_ShowString(1, 1, buf);//�ַ�����ʾ
		sprintf((char *)buf, "Humi:%d.%d %%", data.humi_int, data.humi_deci);//����ת�ַ���   ʪ��
		OLED_ShowString(3, 1, buf);//�ַ�����ʾ

	   key_status = key_setting();
		  switch(key_status)
       {
           case key_status_once:
               beep=1;
               buf1++;
               if(buf1>2) buf1=0;
               delay_ms(100);
               beep=0;
               break;
           case key_status_double:
               beep=1;
               buf1++;
              // if(buf1!=3) buf1;
              //delay_ms(10);
               beep=0;
               break;
           case key_status_long:
               // handle long press
               break;
		   default : break;
       }
		if(key_plus==0 && buf1==1) //�����¶�ֵ+1
		{		
			while(key_plus!=0); 
			{	buf2++;             //����
				beep=1; 
				
			}
			delay_ms(30);
			beep=0;
			if(buf2>100) buf2=100;	
			OLED_Init();		
		}
		if(key_sub==0&&buf1==1) //�����¶�ֵ-1
		{		
			while(key_sub!=0);
			{	buf2--;             //����
				beep=1; 
			}
			delay_ms(30);
			beep=0;
			if(buf2<1) buf2=1;
			OLED_Init();		
		}		
		if(key_plus==0&&buf1==2) //����ʪ��ֵ+1
		{		
			while(key_plus!=0);
			{	buf3++;             //����
				beep=1; 
			}	
			delay_ms(30);
			beep=0;
			if(buf3>100) buf3=100;	
			OLED_Init();	
		}
		if(key_sub==0&&buf1==2) //����ʪ��ֵ-1
		{		
			while(key_sub!=0);
			{	buf3--;             //����
				beep=1; 
			}
			delay_ms(30);
			beep=0;
			if(buf3<1) buf3=1;	
			OLED_Init();		
		}
		sprintf((char *)buf, "Tset:%d C", buf2);//����ת�ַ��� �¶�
		OLED_ShowString(2, 1, buf);//�ַ�����ʾ
		sprintf((char *)buf, "Hset:%d %%", buf3);//����ת�ַ���   ʪ��
		OLED_ShowString(4, 1, buf);//�ַ�����ʾ
	    if(((data.temp_int + 0.1 * data.temp_deci) > buf2) && ((data.humi_int + 0.1 * data.humi_deci) >buf3)) {
				motor_hum=0;
				PWM_Set_fan(data.temp_int, buf2); //������
				PWM_Set_heat(0);
				beep=1,delay_ms(50),beep=0,delay_ms(20),beep=1,delay_ms(20),beep=0;//�¶ȹ��߻���ʪ�ȹ��� ��������������
		}
		else if(((data.temp_int + 0.1 * data.temp_deci) < buf2) && ((data.humi_int + 0.1 * data.humi_deci) >buf3))  {	
				beep=0;
				motor_hum=0;
			 	PWM_Set_fan(0, buf2);//�رշ���
			    PWM_Set_heat(1);//,�������ȵ���
		}
		else if(((data.temp_int + 0.1 * data.temp_deci) < buf2) && ((data.humi_int + 0.1 * data.humi_deci) < buf3)) {
				motor_hum=1,delay_ms(50),motor_hum=1,delay_ms(50);
				PWM_Set_fan(0, buf2);//�رշ���
			    PWM_Set_heat(1);//,�������ȵ��� 11
				beep = 0;
		
		}
		else if (((data.temp_int + 0.1 * data.temp_deci) > buf2) && ((data.humi_int + 0.1 * data.humi_deci) < buf3)){
				motor_hum=1,delay_ms(50),motor_hum=1,delay_ms(50);
				PWM_Set_fan(data.temp_int, buf2);
				PWM_Set_heat(0);
				beep=1,delay_ms(10),beep=0,delay_ms(10),beep=1,delay_ms(10),beep=0;
		}
		else{
				beep = 0;
		}
		if(buf1==1)//��ʾ����״̬
		{
			sprintf((char *)buf, "Tset:%d C", buf2);//����ת�ַ��� �¶�
			OLED_ShowString(2, 1, buf);//�ַ�����ʾ
		}
		else if(buf1==2) //��ʾ����״̬
		{
			sprintf((char *)buf, "Hset:%d %%", buf3);//����ת�ַ��� ʪ��
			OLED_ShowString(4, 1, buf);//�ַ�����ʾ
		}
	}
}