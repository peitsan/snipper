#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stdio.h"
#include "Oled.h"
#include "pwm.h"
//????????

#define key_set PBin(10)//???????
#define key_plus PBin(1)//?????+
#define key_sub PBin(0)//?????-
#define motor_fan PAout(1)//??????
#define motor_heat PAout(2)//????
#define motor_hum PBout(13)//??????
#define beep PBout(12)//??????
#define long_press_time 500 //???????
#define key_speed 50 //??????
#define key_status_once 1//??????
#define key_status_double 2 //?????
#define key_status_long 3 //??????

//????

u8 temp = 0, hum = 0, key_status;
	int buf1=0,buf2=19,buf3=56;
////////////////////////////////////////////////////////////////////////////////// 	 
// ?????????
// buf1:???????????? 0?????? 1:???  2:???
// buf2:??¡­????   0.0~100.0?????
// buf3:??¡­????   0.0%~100.0%
////////////////////////////////////////////////////////////////////////////////// 
	DHT11_Data_TypeDef data;
	unsigned char buf[127] = { 0 };
	int motor_hum_trigger = 0; //??????¡ì??
//??????


void OUTPUT_Init(void)
{ 
 GPIO_InitTypeDef  GPIOA_InitStructure, GPIOB_InitStructure, GPIOH_InitStructure;
 			 //IO??????????
 GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;				 //IO??????????
 GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //???????
 GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_12| DHT11_Out_Pin |GPIO_Pin_13;				 //IO??????????
 GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //???????
 GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 
 GPIO_Init(GPIOA, &GPIOA_InitStructure);
 GPIO_Init(GPIOB, &GPIOB_InitStructure);	

 GPIO_SetBits(GPIOA,GPIO_Pin_2);						 //?????
 GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //?????
 GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //?????
 GPIO_ResetBits(GPIOB,GPIO_Pin_13);						 //????????PB13???????????? ????????????
}

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //??????????	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u8 key_setting(void)
{
    u8 b, c;
    if(key_set == 0) //???????????
    {
        delay_ms(20); //????????
        if(key_set == 0) //???????????
        {
            c = 0;
            while(key_set == 0) //??????????????????
            {
                c++;
                delay_ms(10);
                if(c >= long_press_time) //????
                {
                    return key_status_long;
                }
            }
            //?????????
            for(b = 0; b < key_speed; b++)
            {
                delay_ms(20);
                if(key_set == 0) //????????????
                {
                    delay_ms(10);
                    if(key_set == 0) //????????????????
                    {
                        while(key_set == 0); //??????????
                        return key_status_double; //???
                    }
                }
            }
            //??¡ì?????????????????????
            return key_status_once;
        }
    }
    //??????¡ì???????????0
    return 0;
}
void hum_on(void){
	if (motor_hum_trigger % 3 == 0) { // ?????
          			  motor_hum=1,delay_ms(200),motor_hum=0,delay_ms(400);motor_hum=1;
    }else if (motor_hum_trigger % 3 == 1) { // ?????
          			  motor_hum=1;delay_ms(200),motor_hum=0;
    }	
		motor_hum_trigger ++;
}
void hum_off(void){
	if (motor_hum_trigger % 3 == 1) { // ?????
          			  motor_hum=1,delay_ms(200),motor_hum=0,delay_ms(400);motor_hum=1;
					  motor_hum_trigger = 0;
    }else if (motor_hum_trigger % 3 == 2) { // ?????
          			  motor_hum=1;delay_ms(200),motor_hum=0;
					  motor_hum_trigger = 0;
    }else
		motor_hum_trigger = 0;
}
void refresh(void){
		Read_DHT11(&data);
		delay_ms(2000);
	    if(((data.temp_int + 0.1 * data.temp_deci) > buf2) && ((data.humi_int + 0.1 * data.humi_deci) >buf3)) {
				hum_off();
				motor_fan=1;
				motor_heat=0;
				PWM_Set_fan(data.temp_int, buf2); //??????
				PWM_Set_heat(0);
				beep=1,delay_ms(50),beep=0,delay_ms(20),beep=1,delay_ms(20),beep=0;//??????????????? ??????????????
		}
		else if(((data.temp_int + 0.1 * data.temp_deci) < buf2) && ((data.humi_int + 0.1 * data.humi_deci) >buf3))  {	
				beep=0;
				hum_off();
				motor_fan=1;
				motor_heat=1;
			 	PWM_Set_fan(data.humi_deci, buf3);
			    PWM_Set_heat(1);//,???????????
		}
		else if(((data.temp_int + 0.1 * data.temp_deci) < buf2) && ((data.humi_int + 0.1 * data.humi_deci) < buf3)) {
				hum_on();
				motor_fan=0;
				motor_heat=1;
				PWM_Set_fan(0, buf2);//??????
			    PWM_Set_heat(1);//,??????????? 11
				beep = 0;
		}
		else if (((data.temp_int + 0.1 * data.temp_deci) > buf2) && ((data.humi_int + 0.1 * data.humi_deci) < buf3)){
				hum_on();
				motor_fan=1;
				motor_heat=0;
				PWM_Set_fan(data.temp_int, buf2);
				PWM_Set_heat(0);
				beep=1,delay_ms(10),beep=0,delay_ms(10),beep=1,delay_ms(10),beep=0;
		}
		else{
				beep = 0;
		}
		sprintf((unsigned char *)buf, "Temp:%d.%d C", data.temp_int, data.temp_deci);//?????????? ???
		OLED_ShowString(1, 1, buf);//????????
		sprintf((unsigned char *)buf, "Humi:%d.%d %%", data.humi_int, data.humi_deci);//??????????   ???
		OLED_ShowString(3, 1, buf);//????????
		sprintf((unsigned char *)buf, "Tset:%d C", buf2);//?????????? ???
		OLED_ShowString(2, 1, buf);//????????
		sprintf((unsigned char *)buf, "Hset:%d %%", buf3);//??????????   ???
		OLED_ShowString(4, 1, buf);//????????
		
}
int main(void)
{	 
	DELAY_Init();//???????????????
	OLED_Init() ; 
	PWM_Init();	
	KEY_Init();
	OUTPUT_Init();
	refresh();
	while(1)
	{
	   refresh();
	   key_status = key_setting();
		  switch(key_status)
       {
           case key_status_once:
               beep=1;
               buf1++;
               if(buf1>2) buf1=0;
			 	refresh();
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
		if(key_plus==0 && buf1==1) //????????+1
		{		
			while(key_plus!=0); 
			{	buf2++;             //????
				beep=1; 
			}
			refresh();
			delay_ms(30);
			beep=0;
			if(buf2>100) buf2=100;	
		}
		if(key_sub==0&&buf1==1) //????????-1
		{		
			while(key_sub!=0);
			{	buf2--;             //????
				beep=1; 
			}
			delay_ms(30);
			refresh(); 
			beep=0;
			if(buf2<1) buf2=1;
		}		
		if(key_plus==0&&buf1==2) //????????+1
		{		
			while(key_plus!=0);
			{	buf3++;             //????
				beep=1; 
			}	
			refresh();
			delay_ms(30);
			beep=0;
			if(buf3>100) buf3=100;	
		}
		if(key_sub==0&&buf1==2) //????????-1
		{		
			while(key_sub!=0);
			{	buf3--;             //????
				beep=1; 
			}
			refresh();
			delay_ms(30);
			beep=0;
			if(buf3<1) buf3=1;	
		}
		if(buf1==1)//?????????
		{
			sprintf((char *)buf, "Tset:%d C", buf2);//?????????? ???
			OLED_ShowString(2, 1, buf);//????????
		}
		else if(buf1==2) //?????????
		{
			sprintf((char *)buf, "Hset:%d %%", buf3);//?????????? ???
			OLED_ShowString(4, 1, buf);//????????
		}
	
	}
}