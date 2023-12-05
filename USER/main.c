#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "dth11.h"
#include "stdio.h"
#include "Oled.h"
//头文件引用

// #define key_set PBin(11)//设置按键
// #define key_plus PBin(12)//温湿度+
// #define key_sub PBin(13)//温湿度-
#define key_set PBin(10)//设置按键
#define key_plus PBin(1)//温湿度+
#define key_sub PBin(0)//温湿度-
// #define motor_hum PBout(10)//加湿电机
// #define motor_fan PBout(14)//散热风扇
// #define beep PBout(15)//蜂鸣器
#define motor_hum PAout(2)//加湿电机
#define motor_fan PAout(1)//散热风扇
#define beep PBout(12)//蜂鸣器
#define long_press_time 500 //长按时间
#define key_speed 50 //双击时间

#define key_status_once 1//单击状态
#define key_status_double 2 //双击状态
#define key_status_long 2 //长按状态
//宏定义



void OUTPUT_Init(void)
{ 
 GPIO_InitTypeDef  GPIOA_InitStructure, GPIOB_InitStructure;
 	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能P端口时钟
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能P端口时钟	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_10;				 //IO输出端口配置
 GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;				 //IO输出端口配置
 GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_12| IO_DHT11;				 //IO输出端口配置
 GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIOA_InitStructure);
 GPIO_Init(GPIOB, &GPIOB_InitStructure);	

 GPIO_SetBits(GPIOA,GPIO_Pin_2);						 //输出高
 GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //输出高
 GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //输出高
}

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u8 key_setting(void)
{
	u8 b,c;
	if(key_set==0) //设置按键按下
	{	
		c = 0;
		delay_ms(20);
		if(key_set==0) //设置按键消抖
		{	
			while((key_set==0)&&(c < long_press_time))
			{
				c ++;
				delay_ms(10);
			}
			if( c >= long_press_time) //长按
			{
				return key_status_long;
			}
			else //短按
			{
				for(b = 0; b < key_speed; b++)
				{
					delay_ms(20);
					if(key_set==0) //双击按键消抖
					{
						delay_ms(10);
						if(key_set==0) //双击按键消抖
						{
							return key_status_double;
						}
					}
				}
				return key_status_once;
			}
		}
	}
}

int main(void)
{	 
		 

	u8 temp = 0, hum = 0;
	int buf1=0,buf2=30,buf3=20;
////////////////////////////////////////////////////////////////////////////////// 	 
// 参数说明：
// buf1:控制启动开关 0：锁定 1:温度  2:湿度
// buf2:设定温度值   0.0~100.0摄氏度
// buf3:设定湿度值   0.0%~100.0%
////////////////////////////////////////////////////////////////////////////////// 
	DHT11_Data_TypeDef data;
	unsigned char buf[127] = { 0 };
	DELAY_Init();//下面都是初始化函数
	OLED_Init() ; 	
	DHT11_Init();
	KEY_Init();
	OUTPUT_Init();
	while(1)
	{
		DHT11_Read_Data(&temp, &hum);//温湿度读取
		Read_DHT11(&data);
		// printf("temp:%d hum:%d", temp, hum);//串口打印
		// sprintf((char *)buf, "Temp:%d.%d C", data.temp_int, data.temp_deci);//整数转字符串 温度
		sprintf((char *)buf, "Temp:%dC", temp);//整数转字符串 温度
		OLED_ShowString(1, 1, buf);//字符串显示
		// sprintf((char *)buf, "Humi:%d.%d", data.humi_int, data.humi_deci);//整数转字符串   湿度
		sprintf((char *)buf, "Humi:%d",temp);//整数转字符串   湿度
		OLED_ShowString(3, 1, buf);//字符串显示

		u8 key_status = key_setting();
		switch(key_status)
		{
			case key_status_once:
				beep=1;
				buf1++;
				if(buf1>3) buf1=0;
				delay_ms(10);
				beep=0;
			case key_status_double:
				beep=1;
				buf1++;
				if(buf1>3) buf1=0;
				delay_ms(10);
				beep=0;

		}
		if(key_plus==0 && buf1==1) //期望温度值+1
		{		
			while(key_plus!=0); 
			{	buf2++;             //防抖
				beep=1; 
				delay_ms(30);
			}
			beep=0;
			if(buf2>100) buf2=100,OLED_Init();			
		}
		if(key_sub==0&&buf1==1) //期望温度值-1
		{		
			while(key_sub!=0);
			{	buf2--;             //防抖
				beep=1; 
			}
			delay_ms(10);
			beep=0;
			if(buf2<1) buf2=1,OLED_Init();			
		}
		
		if(key_plus==0&&buf1==2) //期望湿度值+1
		{		
			while(key_plus!=0);
			{	buf3++;             //防抖
				beep=1; 
			}	
			delay_ms(10);
			beep=0;
			if(buf3>100) buf3=100,OLED_Init();			
		}

		if(key_sub==0&&buf1==2) //期望湿度值-1
		{		
			while(key_sub!=0);
			{	buf3--;             //防抖
				beep=1; 
			}
			delay_ms(10);
			beep=0;
			if(buf3<1) buf3=1,OLED_Init();			
		}
		sprintf((char *)buf, "Tset:%d", buf2);//整数转字符串 温度
		OLED_ShowString(2, 1, buf);//字符串显示
		sprintf((char *)buf, "Hset:%d", buf3);//整数转字符串   湿度
		OLED_ShowString(4, 1, buf);//字符串显示

			
		// if(data.temp_int>buf2||data.humi_int<buf3)  beep=1;//温度过高或者湿度过低 开启蜂鸣器提醒
		// else beep=0;
		// if(data.temp_int>buf2) motor_fan=0;//温度大于设置值  开启散热
		// else motor_fan=1;
		// if(data.humi_int<buf3) motor_hum=0;//湿度过低  开启加湿
		// else motor_hum=1;
		if(temp >buf2||hum <buf3)  beep=0;//温度过高或者湿度过低 开启蜂鸣器提醒
		else beep=0;
		if(temp >buf2) motor_fan=0;//温度大于设置值  开启散热
		else motor_fan=1;
		if(hum<buf3) motor_hum=0;//湿度过低  开启加湿
		else motor_hum=1;
		if(buf1==1)//显示设置状态
		{
			sprintf((char *)buf, "Tset:%dC", 1);//整数转字符串 温度
			OLED_ShowString(2, 1, buf);//字符串显示
		}
		else if(buf1==2) //显示设置状态
		{
			sprintf((char *)buf, "Hset:%d ", 1);//整数转字符串 湿度
			OLED_ShowString(4, 1, buf);//字符串显示
		}

		
	}
}


