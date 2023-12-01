#include "DTH11.h"
#include "stm32f10x_conf.h"
void delay(int32_t us)
{
	while(us--){}
}

void DHT11_IO_IN(void)//��ʪ��ģ�����뺯��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=IO_DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);
}

void DHT11_IO_OUT(void)//��ʪ��ģ���������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=IO_DHT11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);
}
//��λDHT11
void DHT11_Rst(void)   
{                 
    DHT11_IO_OUT(); //SET OUTPUT
    DHT11_DQ_High ;
    DHT11_DQ_Low; //DQ=0
    delay(25000);    //��������18ms
    DHT11_DQ_High; //DQ=1 
    delay(55);     //��������20~40us
}
//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check(void)    
{   
	u8 retry=0;//������ʱ����
	DHT11_IO_IN();    //SET INPUT 
    if (GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1)//DHT11������40~80us
		{
			return 0 ;
		}else
		{
			while(GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==0 && (retry < 100))retry++ ;
		}
		delay(40) ;
		delay(40) ;
		return 1 ;
}
//��DHT11��ȡһ��λ
//����ֵ��1/0
u8 DHT11_Read_Bit(void)  
{
	u8 retry=0;
	while((GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1)&&retry<50)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		delay(2);
	}
	if(retry>45)  return -1;//��ʱ��� ��������
	retry=0;
	
	while((GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==0)&&retry<50)//�ȴ���ߵ�ƽ
	{
		retry++;
		delay(2);
	}
	delay(40);//�ȴ�40us
	if(GPIO_ReadInputDataBit(GPIO_DHT11,IO_DHT11)==1)
		return 1;
	else 
		return 0;   
}
//��DHT11��ȡһ���ֽ�
//����ֵ������������
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

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
  u8 buf[5];// ʪ��С������֮�ࡣ��������
  u8 i;
  DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
				buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])//У��
		{
				*humi=buf[0];//�õ�ʪ��
				*temp=buf[2];//�õ��¶�
		}
		}else return 1;
		return 0;    
}

//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���

//����1:������

//����0:����     

void DHT11_Init(void)
{     
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 DHT11_Rst();  //��λDHT11
 DHT11_Check();//�ȴ�DHT11�Ļ�Ӧ
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
			delay(30);
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
//int main(void)
//{
//   u8 wd=0;      
//   u8 sd=0;
// 
//   RCC_Configuration();//ϵͳʱ�ӳ�ʼ��
//   GPIO_Configuration();//�˿ڳ�ʼ��
//   USART_Configuration();
//   NVIC_Configuration();
// 
//   DHT11_Init();
//   
//   while(1)
//   {
//  DHT11_Read_Data(&wd,&sd);//��ȡ��ʪ��ֵ    
//  printf("��ǰ�����¶�: %d ��\r\n",wd);    
//  printf("��ǰ����ʪ��: %d %%\r\n",sd);    
//  delay_ms(1000);
//  delay_ms(1000);
//   }
//}
// 
//void RCC_Configuration(void)
//{
//    SystemInit();//72m
//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//}
// 
//void GPIO_Configuration(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX
//GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
//GPIO_Init(GPIOA,&GPIO_InitStructure);
// 
//GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
//GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//GPIO_Init(GPIOA,&GPIO_InitStructure);
//}
// 
//void NVIC_Configuration(void)
//{
//   NVIC_InitTypeDef NVIC_InitStructure; 
// 
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
// 
//NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
//NVIC_Init(&NVIC_InitStructure);
//}
// 
//void USART_Configuration(void)
//{
//    USART_InitTypeDef  USART_InitStructure;
// 
//USART_InitStructure.USART_BaudRate=9600;
//USART_InitStructure.USART_WordLength=USART_WordLength_8b;
//USART_InitStructure.USART_StopBits=USART_StopBits_1;
//USART_InitStructure.USART_Parity=USART_Parity_No;
//USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
//USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
// 
//USART_Init(USART1,&USART_InitStructure);
//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
//USART_Cmd(USART1,ENABLE);
//USART_ClearFlag(USART1,USART_FLAG_TC);
//}
