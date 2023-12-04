#ifndef _DTH11_H_
#define _DTH11_H_

   #include "stm32f10x.h"

#define IO_DHT11           GPIO_Pin_14 //�����м������������ֲ

#define GPIO_DHT11         GPIOB    //�����м������������ֲ

////////////////////////////////////////////////////////// 

#define DHT11_DQ_High    GPIO_SetBits(GPIO_DHT11,IO_DHT11) 

#define DHT11_DQ_Low     GPIO_ResetBits(GPIO_DHT11,IO_DHT11)

#define DHT11_Out_Pin		GPIO_Pin_0
#define DHT11_Out_RCC		RCC_APB2Periph_GPIOA

////////////////////////////// A��
#define DHT11		GPIOA

#define HIGH  1
#define LOW   0
#define DHT11_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(DHT11, DHT11_Out_Pin);\
					else		\
					GPIO_ResetBits(DHT11, DHT11_Out_Pin)
					
#define  DHT11_DATA_IN()	   GPIO_ReadInputDataBit(DHT11, DHT11_Out_Pin)

typedef struct
{
	uint8_t  humi_int;		
	uint8_t  humi_deci;	 	
	uint8_t  temp_int;
	uint8_t  temp_deci;
	uint8_t  check_sum;	 	
		                 
}DHT11_Data_TypeDef;

void DHT11_GPIO_Config(void);

static void DHT11_Mode_IPU(void);

static void DHT11_Mode_Out_PP(void);
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);
static uint8_t Read_Byte(void);


void delay(int32_t us)  ;

void DHT11_IO_OUT(void);//��ʪ��ģ���������

void DHT11_IO_IN(void); //��ʪ��ģ�����뺯��

void DHT11_Init(void);  //��ʼ��DHT11

u8   DHT11_Read_Data(u8 *temp,u8 *humi); //��ȡ��ʪ��

u8   DHT11_Read_Byte(void);              //����һ���ֽ�

u8   DHT11_Read_Bit(void);               //����һ��λ

u8   DHT11_Check(void);                  //����Ƿ����DHT11

void DHT11_Rst(void);                    //��λDHT11    
uint8_t DHT_ByteRead(unsigned char *dat) ;
#endif

