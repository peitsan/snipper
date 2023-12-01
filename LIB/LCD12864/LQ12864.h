#ifndef __LQ12864_H__
#define __LQ12864_H__
 #include "stm32f10x_conf.h"
//--------------------¶¨ÒåIO¶Ë¿Ú-----------------//
#define LCD_DC(x)       GPIO_WriteBit(GPIOB,GPIO_Pin_6,x)
#define LCD_RST(x)      GPIO_WriteBit(GPIOB,GPIO_Pin_7,x)
#define LCD_SDA(x)	    GPIO_WriteBit(GPIOB,GPIO_Pin_8,x)
#define LCD_SCL(x)	    GPIO_WriteBit(GPIOB,GPIO_Pin_9,x)
extern void LCD_Init(void);
extern void LCD_CLS(void);
extern void LCD_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void LCD_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void LCD_P14x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
extern void LCD_Print(unsigned char x, unsigned char y, unsigned char ch[]);
extern void LCD_PutPixel(unsigned char x,unsigned char y);
extern void LCD_Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char gif);
extern void Draw_LQLogo(void);
extern void Draw_LibLogo(const unsigned char *logo);
extern void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char * bmp); 
extern const unsigned char LIBLOGO60x58[480];


#endif

