#include "DTH11.h"
#include "Delay.h"
#include "stm32f10x_conf.h"

void DHT11_IO_IN(void)//温湿度模块输入函数
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=IO_DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);
}

void DHT11_IO_OUT(void)//温湿度模块输出函数
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=IO_DHT11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);
}
//复位DHT11
void DHT11_Rst(void)   
{                 
    DHT11_IO_OUT(); //SET OUTPUT
    DHT11_DQ_High ;
    DHT11_DQ_Low; //DQ=0
    delay_ms(20);    //拉低至少18ms
    DHT11_DQ_High; //DQ=1 
    delay_us(35);     //主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check(void)    
{   
	u8 retry=0;//定义临时变量
	DHT11_IO_IN();    //SET INPUT 
    if (GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1)//DHT11会拉低40~80us
		{
			return 0 ;
		}else
		{
			while(GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==0 && (retry < 100))retry++ ;
		}
		delay_us(40) ;
		delay_us(40) ;
		return 1 ;
}
//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void)  
{
	u8 retry=0;
	while((GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1)&&retry<50)//等待变为低电平
	{
		retry++;
		delay_us(2);
	}
	if(retry>45)  return -1;//超时检测 结束程序
	retry=0;
	
	while((GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==0)&&retry<50)//等待变高电平
	{
		retry++;
		delay_us(2);
	}
	delay_us(40);//等待40us
	if(GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1)
		return 1;
	else 
		return 0;   
}
//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
		for (i=0;i<8;i++) 
		{
			dat<<=1; 
			dat|=DHT11_Read_Bit();
    }    
    return dat;

}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
  u8 buf[5];// 湿度小数整数之类。。。。。
  u8 i;
  DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
				buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])//校验
		{
				*humi=buf[0];//得到湿度
				*temp=buf[2];//得到温度
		}
		}else return 1;
		return 0;    
}

//初始化DHT11的IO口 DQ 同时检测DHT11的存在

//返回1:不存在

//返回0:存在     

void DHT11_Init(void)
{     
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 DHT11_Rst();  //复位DHT11
 DHT11_Check();//等待DHT11的回应
}

uint8_t DHT_ByteRead(unsigned char *dat)
{	unsigned char temp=0;
	unsigned char x,y;
	unsigned char m=0;
	unsigned char n=0;
	unsigned char mask=0x01;
	unsigned char sum=0;
		DHT11_IO_IN() ;
	for(y=0;y<5;y++)
	{
		for(mask=0x80;mask!=0;mask>>=1)
		{	
			while(GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==0&&m<50)m++;
			delay_us(30);
			if(GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1)
				temp|=mask;
			else
				temp&=(~mask);
			while(GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1&&n<50)n++;
		}
		*(dat+y)=temp;
		temp=0;
	}
	for(x=0;x<4;x++)
		sum+=*(dat+x);	
	if((sum&=0xff)==*(dat+4))
		return 1;
	else 
		return 0;
}
/////////////////////////////////////////////////////
 
// void DHT11_GPIO_Config(void)
// {		
// 	GPIO_InitTypeDef GPIO_InitStructure;

// 	RCC_APB2PeriphClockCmd(DHT11_Out_RCC, ENABLE); 
//   	GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;	
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
//   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//   	GPIO_Init(DHT11, &GPIO_InitStructure);		  

// 	GPIO_SetBits(DHT11, DHT11_Out_Pin);	 
// }

// static void DHT11_Mode_IPU(void)
// {
//  	  GPIO_InitTypeDef GPIO_InitStructure;
// 	  GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;
// 	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
// 	  GPIO_Init(DHT11, &GPIO_InitStructure);	 
// }

static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Pin = DHT11_Out_Pin;	

  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  	GPIO_Init(DHT11, &GPIO_InitStructure);	 	 
}


uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	DHT11_IO_IN();
	DHT11_DATA_OUT(LOW);
	delay_ms(20);

	DHT11_DATA_OUT(HIGH); 

	delay_us(35);  

	DHT11_IO_IN();

	if(DHT11_DATA_IN() == Bit_RESET)     
	{
		while(DHT11_DATA_IN() == Bit_RESET);

		while(DHT11_DATA_IN() == Bit_SET);

		DHT11_Data -> humi_int = DHT11_Read_Byte();

		DHT11_Data -> humi_deci = DHT11_Read_Byte();

		DHT11_Data -> temp_int = DHT11_Read_Byte();

		DHT11_Data -> temp_deci = DHT11_Read_Byte();

		DHT11_Data -> check_sum= DHT11_Read_Byte();


		DHT11_Mode_Out_PP();
		DHT11_DATA_OUT(HIGH);

		if (DHT11_Data -> check_sum == DHT11_Data -> humi_int + DHT11_Data -> humi_deci + DHT11_Data -> temp_int + DHT11_Data -> temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	else
	{		
		return ERROR;
	}   
}
