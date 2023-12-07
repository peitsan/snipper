#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stdio.h"
#include "Oled.h"
#include "pwm.h"
//头文件引用

#define key_set PBin(10)//设置按键
#define key_plus PBin(1)//温湿度+
#define key_sub PBin(0)//温湿度-
#define motor_fan PAout(1)//散热风扇
#define motor_heat PAout(2)//加熱片
#define motor_hum PBout(13)//加湿电机
#define beep PBout(12)//蜂鸣器
#define long_press_time 500 //长按时间
#define key_speed 50 //双击时间
#define key_status_once 1//单击状态
#define key_status_double 2 //双击状态
#define key_status_long 3 //长按状态

//宏定义

u8 temp = 0, hum = 0, key_status;
	int buf1=0,buf2=19,buf3=56;
////////////////////////////////////////////////////////////////////////////////// 	 
// 参数说明：
// buf1:控制启动开关 0：锁定 1:温度  2:湿度
// buf2:设定温度值   0.0~100.0摄氏度
// buf3:设定湿度值   0.0%~100.0%
////////////////////////////////////////////////////////////////////////////////// 
	DHT11_Data_TypeDef data;
	char buf[127] = { 0 };
	int motor_hum_trigger = 0; //加湿模式切换
//全局变量


void OUTPUT_Init(void)
{ 
 GPIO_InitTypeDef  GPIOA_InitStructure, GPIOB_InitStructure, GPIOH_InitStructure;
 	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能P端口时钟
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能P端口时钟	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_10;				 //IO输出端口配置
 GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;				 //IO输出端口配置
 GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_12| DHT11_Out_Pin |GPIO_Pin_13;				 //IO输出端口配置
 GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 
//  GPIOH_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //IO输出端口配置
//  GPIOH_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //下降沿触发输入
//  GPIOH_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIOA_InitStructure);
 GPIO_Init(GPIOB, &GPIOB_InitStructure);	
//  GPIO_Init(GPIOB, &GPIOH_InitStructure);

 GPIO_SetBits(GPIOA,GPIO_Pin_2);						 //输出高
 GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //输出高
 GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //输出高
 GPIO_ResetBits(GPIOB,GPIO_Pin_13);						 //输出低，将PB13设置为输出低电平 给加湿器提供信号
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
    u8 b, c;
    if(key_set == 0) //设置按键按下
    {
        delay_ms(20); //延时去抖动
        if(key_set == 0) //设置按键消抖
        {
            c = 0;
            while(key_set == 0) //按键被按下，开始计数
            {
                c++;
                delay_ms(10);
                if(c >= long_press_time) //长按
                {
                    return key_status_long;
                }
            }
            //按键已释放
            for(b = 0; b < key_speed; b++)
            {
                delay_ms(20);
                if(key_set == 0) //第二次按键操作
                {
                    delay_ms(10);
                    if(key_set == 0) //第二次按键操作消抖
                    {
                        while(key_set == 0); //等待按键释放
                        return key_status_double; //双击
                    }
                }
            }
            //没有第二次按键操作，返回单击
            return key_status_once;
        }
    }
    //按键没有被按下，返回0
    return 0;
}
void hum_on(void){
	if (motor_hum_trigger % 3 == 0) { // 七彩模式
          			  motor_hum=1,delay_ms(1000),motor_hum=0,delay_ms(1000);motor_hum=1;
    }else if (motor_hum_trigger % 3 == 1) { // 七彩模式
          			  motor_hum=1;
    }	
		motor_hum_trigger ++;
}
void hum_off(void){
	if (motor_hum_trigger % 3 != 0) { // 七彩模式
          			   motor_hum=0;
    }
		motor_hum_trigger = 0;
}
void refres h(void){
		Read_DHT11(&data);
	    if(((data.temp_int + 0.1 * data.temp_deci) > buf2) && ((data.humi_int + 0.1 * data.humi_deci) >buf3)) {
				hum_off();
				motor_fan=1;
				motor_heat=0;
				PWM_Set_fan(data.temp_int, buf2); //开风扇
				PWM_Set_heat(0);
				beep=1,delay_ms(50),beep=0,delay_ms(20),beep=1,delay_ms(20),beep=0;//温度过高或者湿度过低 开启蜂鸣器提醒
		}
		else if(((data.temp_int + 0.1 * data.temp_deci) < buf2) && ((data.humi_int + 0.1 * data.humi_deci) >buf3))  {	
				beep=0;
				hum_off();
				motor_fan=1;
				motor_heat=1;
			 	PWM_Set_fan(data.humi_deci, buf3);
			    PWM_Set_heat(1);//,开启加热电阻
		}
		else if(((data.temp_int + 0.1 * data.temp_deci) < buf2) && ((data.humi_int + 0.1 * data.humi_deci) < buf3)) {
				hum_on();
				motor_fan=0;
				motor_heat=1;
				PWM_Set_fan(0, buf2);//关闭风扇
			    PWM_Set_heat(1);//,开启加热电阻 11
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
		sprintf((char *)buf, "Temp:%d.%d C", data.temp_int, data.temp_deci);//整数转字符串 温度
		OLED_ShowString(1, 1, buf);//字符串显示
		sprintf((char *)buf, "Humi:%d.%d %%", data.humi_int, data.humi_deci);//整数转字符串   湿度
		OLED_ShowString(3, 1, buf);//字符串显示
		sprintf((char *)buf, "Tset:%d C", buf2);//整数转字符串 温度
		OLED_ShowString(2, 1, buf);//字符串显示
		sprintf((char *)buf, "Hset:%d %%", buf3);//整数转字符串   湿度
		OLED_ShowString(4, 1, buf);//字符串显示
}
int main(void)
{	 
	DELAY_Init();//下面都是初始化函数
	OLED_Init() ; 
	PWM_Init();	
	KEY_Init();
	OUTPUT_Init();
	refresh();
	while(1)
	{
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
		if(key_plus==0 && buf1==1) //期望温度值+1
		{		
			while(key_plus!=0); 
			{	buf2++;             //防抖
				beep=1; 
			}
			refresh();
			delay_ms(30);
			beep=0;
			if(buf2>100) buf2=100;	
			// OLED_Init();		
		}
		if(key_sub==0&&buf1==1) //期望温度值-1
		{		
			while(key_sub!=0);
			{	buf2--;             //防抖
				beep=1; 
			}
			delay_ms(30);
			refresh(); 
			beep=0;
			if(buf2<1) buf2=1;
			// OLED_Init();		
		}		
		if(key_plus==0&&buf1==2) //期望湿度值+1
		{		
			while(key_plus!=0);
			{	buf3++;             //防抖
				beep=1; 
			}	
			refresh();
			delay_ms(30);
			beep=0;
			if(buf3>100) buf3=100;	
			// OLED_Init();	
		}
		if(key_sub==0&&buf1==2) //期望湿度值-1
		{		
			while(key_sub!=0);
			{	buf3--;             //防抖
				beep=1; 
			}
			refresh();
			delay_ms(30);
			beep=0;
			if(buf3<1) buf3=1;	
			// OLED_Init();		
		}
		if(buf1==1)//显示设置状态
		{
			sprintf((char *)buf, "Tset:%d C", buf2);//整数转字符串 温度
			OLED_ShowString(2, 1, buf);//字符串显示
		}
		else if(buf1==2) //显示设置状态
		{
			sprintf((char *)buf, "Hset:%d %%", buf3);//整数转字符串 湿度
			OLED_ShowString(4, 1, buf);//字符串显示
		}
	
	}
}