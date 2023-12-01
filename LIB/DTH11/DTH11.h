#ifndef _DTH11_H_
#define _DTH11_H_

   #include "stm32f10x.h"

#define IO_DHT11           GPIO_Pin_15 //�����м������������ֲ

#define GPIO_DHT11         GPIOC    //�����м������������ֲ

 

#define DHT11_DQ_High    GPIO_SetBits(GPIO_DHT11,IO_DHT11) 

#define DHT11_DQ_Low     GPIO_ResetBits(GPIO_DHT11,IO_DHT11)

 
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

