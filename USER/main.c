#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "dth11.h"
#include "stdio.h"
#include "Oled.h"
//头文件引用

#define key_set PBin(11)//设置按键
#define key_plus PBin(12)//温湿度+
#define key_sub PBin(13)//温湿度-
#define motor_hum PBout(10)//加湿电机
#define motor_fan PBout(14)//散热风扇
#define beep PBout(15)//蜂鸣器
//宏定义

void OUTPUT_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能P端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_10;				 //IO输出端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 GPIO_SetBits(GPIOB,GPIO_Pin_14);						 //输出高
 GPIO_SetBits(GPIOB,GPIO_Pin_15);						 //输出高
GPIO_SetBits(GPIOB,GPIO_Pin_10);						 //输出高
}
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
int main(void)
{	 
	int buf1=0,buf2=30,buf3=30;
	u8 temp = 0, hum = 0;
	unsigned char buf[100] = { 0 };
	DELAY_Init();//下面都是初始化函数
	OLED_Init() ; 	
	DHT11_Init();
	KEY_Init();
	OUTPUT_Init();
	while(1)
	{
		DHT11_Read_Data(&temp, &hum);//温湿度读取
		sprintf((char *)buf, "温度:%d C  ", temp);//整数转字符串 温度
		OLED_ShowString(2, 1, buf);//字符串显示
		sprintf((char *)buf, "湿度:%d   " , hum);//整数转字符串   湿度
		OLED_ShowString(2, 3, buf);//字符串显示

		if(key_set==1) //设置按键
		{	
			while(key_set!=0);
				buf1++;
			if(buf1>1) buf1=0;
		}
		if(key_plus==1&&buf1==0) //期望温度值+1
		{		
			while(key_plus!=0);
				buf2++;	
			if(buf2>100) buf2=100,OLED_Init();			
		}
		if(key_sub==1&&buf1==0) //期望温度值-1
		{		
			while(key_sub!=0);
				buf2--;	
			if(buf2<1) buf2=1,OLED_Init();			
		}
		
		if(key_plus==1&&buf1==1) //期望湿度值+1
		{		
			while(key_plus!=0);
				buf3++;		
			if(buf3>100) buf3=100,OLED_Init();			
		}

		if(key_sub==1&&buf1==1) //期望湿度值-1
		{		
			while(key_sub!=0);
				buf3--;		
			if(buf3<1) buf3=1,OLED_Init();			
		}
		sprintf((char *)buf, "预设:%d", buf2);//整数转字符串 温度
		OLED_ShowString(80, 1, buf);//字符串显示
		sprintf((char *)buf, "预设:%d" , buf3);//整数转字符串   湿度
		OLED_ShowString(80, 3, buf);//字符串显示

			
		if(temp>buf2||hum<buf3)  beep=0;//温度过高或者湿度过低 开启蜂鸣器提醒
		else beep=1;
		if(temp>buf2) motor_fan=0;//温度大于设置值  开启散热
		else motor_fan=1;
		if(hum<buf3) motor_hum=0;//湿度过低  开启加湿
		else motor_hum=1;
		if(buf1==0)//显示设置状态
		{
			sprintf((char *)buf, "预设温度:%d", 1);//整数转字符串 温度
			OLED_ShowString(30, 6, buf);//字符串显示
		}
		else //显示设置状态
		{
			sprintf((char *)buf, "预设湿度:%d ", 1);//整数转字符串 湿度
			OLED_ShowString(30, 6, buf);//字符串显示
		}
		
	}
}


