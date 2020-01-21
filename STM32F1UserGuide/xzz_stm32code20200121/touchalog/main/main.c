#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "lcd.h"
#include "stdio.h"


/*IO模拟SPI初始化*/
void AnalogSpi_init(void);

/*SPI写控制字*/
static void XPT2046_WriteCMD ( uint8_t ucCmd );

/*SPI读数据，读取的是12个位，所以循环12次*/
static uint16_t XPT2046_ReadCMD(void);

/*采集触摸屏原始数据
* ucChannel = 0x90 :通道 Y+的选择控制字
* ucChannel =0xd0 :通道 X+的选择控制字
* return 该通道的 ADC 采样结果
*/
static uint16_t XPT2046_ReadAdc ( uint8_t ucChannel );

/*
* 读取 XPT2046 的 X 通道和 Y 通道的 AD 值（ 12 bit，最大是 4096）
* X_Ad ：存放 X 通道 AD 值的地址
* Y_Ad ：存放 Y 通道 AD 值的地址
*/
static void XPT2046_ReadAdc_XY ( uint16_t * X_Ad, uint16_t * Y_Ad );

/*对获取的X_Ad和Y_Ad进行算法滤波得到真正的X,Y值
*X:真正滤波后的x值返回
*Y:真正滤波后的y值返回
*/
void XPT2046_ReadAdc_Smooth_XY(uint16_t *X , uint16_t *Y);

int main(void)
{
	uint16_t Xvalue = 0, Yvalue = 0;
	
	int time = 0;
	
	int tx1 = 0 , ty1 = 0;
	int tx2 = 0 , ty2 = 0;
	int tx3 = 0 , ty3 = 0;
	int tx4 = 0 , ty4 = 0;
			
	int centreX = 0, centreY = 0;
	float Xfac = 0,Yfac = 0,Xoffset = 0, Yoffset = 0,Xfacbase = 0, Yfacbase = 0; 
	int X = 0, Y = 0;
	
	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	LCD_Init();//初始化LCD
	LCD_DisplayOn();
	AnalogSpi_init();//模拟SPI IO初始化
	delay_ms(3000);
	printf("xxxzzzzzz\r\n");
	
	LCD_Clear(WHITE);
	
	LCD_Fill(20,20,30,30,RED);
	delay_ms(2000);
	for(time = 0;time<5;time++)
	{
		XPT2046_ReadAdc_Smooth_XY( &Xvalue, &Yvalue );
		delay_ms(1000);
		printf("x1 = %d y1 = %d\r\n",Xvalue,Yvalue);
		if(Xvalue > 0)
		{
			tx1 = Xvalue;
			ty1 = Yvalue;
		}
			
	}
	
	Xvalue = 0;
	Yvalue = 0;
	
	LCD_Fill(200,20,210,30,RED);
	delay_ms(2000);
	for(time = 0;time<5;time++)
	{
		XPT2046_ReadAdc_Smooth_XY( &Xvalue, &Yvalue );
		delay_ms(1000);
		printf("x2 = %d y2 = %d\r\n",Xvalue,Yvalue);
		
		if(Xvalue > 0)
		{
			tx2 = Xvalue;
			ty2 = Yvalue;
		}
	}
	
	Xvalue = 0;
	Yvalue = 0;
	
	LCD_Fill(20,300,30,310,RED);
	delay_ms(2000);
	for(time = 0;time<5;time++)
	{
		XPT2046_ReadAdc_Smooth_XY( &Xvalue, &Yvalue );
		delay_ms(1000);
		printf("x3 = %d y3 = %d\r\n",Xvalue,Yvalue);
		
		if(Xvalue > 0)
		{
			tx3 = Xvalue;
			ty3 = Yvalue;
		}
	}
	
	Xvalue = 0;
	Yvalue = 0;
	
	LCD_Fill(200,300,210,310,RED);
	delay_ms(2000);
	for(time = 0;time<5;time++)
	{
		XPT2046_ReadAdc_Smooth_XY( &Xvalue, &Yvalue );
		delay_ms(1000);
		printf("x4 = %d y4 = %d\r\n",Xvalue,Yvalue);
		
		if(Xvalue > 0)
		{
			tx4 = Xvalue;
			ty4 = Yvalue;
		}
	}
	
	Xvalue = 0;
	Yvalue = 0;
	
//	LCD_Fill(100,150,110,160,RED);
//	delay_ms(2000);
//	for(time = 0;time<5;time++)
//	{
//		XPT2046_ReadAdc_Smooth_XY( &Xvalue, &Yvalue );
//		delay_ms(1000);
//		printf("centreX = %d centreY = %d\r\n",Xvalue,Yvalue);
//		
//		if(Xvalue > 0)
//		{
//			centreX = Xvalue;
//			centreY = Yvalue;
//		}
//	}
//	
//	Xvalue = 0;
//	Yvalue = 0;
	
	Xfacbase = (float)((200-20)+(200-20))/2.0;
	Xfac = Xfacbase/(float)(((tx2-tx1) + (tx4-tx3))/2.0);
	
	Yfacbase = (float)((300-20)+(300-20))/2.0;
	Yfac = Yfacbase/(float)(((ty3-ty1)+(ty4-ty2))/2.0);
	
	printf("Xfac = %f  Yfac = %f \r\n",Xfac,Yfac);
		
	Xoffset = (float)((240-Xfac*(tx2+tx1))/2);
	Yoffset = (float)((320-Yfac*(ty3+ty1))/2);
	
	printf("Xoffset = %f  Yoffset = %f \r\n",Xoffset,Yoffset);
		
	while(1)
	{
		
		XPT2046_ReadAdc_Smooth_XY( &Xvalue, &Yvalue );	
		printf("X value = %d Y value = %d\r\n",Xvalue,Yvalue);
		
		if(Xvalue>0)
		{
			X = 0;
			Y = 0;
			
			X = Xfac*Xvalue+Xoffset;
			Y = Yfac*Yvalue+Yoffset;

			printf("X = %d Y = %d \r\n",X,Y);
			LCD_Fill(X,Y,X+10,Y+10,RED);
			Xvalue = 0;
			Yvalue = 0;
			
		}
				
		delay_ms(100);		
	}
	return 0;
	
}








/*IO模拟SPI初始化*/
void AnalogSpi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_0|GPIO_Pin_13;//初始PC0(SCLK),PC3(MOSI),PC13(CS)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;//初始PC1(中断PEN),PC2(MISO)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}



/*SPI写控制字*/
static void XPT2046_WriteCMD ( uint8_t ucCmd )
{
	uint8_t i;

	GPIO_ResetBits(GPIOC,GPIO_Pin_3); //MOSI=0
	GPIO_ResetBits (GPIOC,GPIO_Pin_0); //SCLK=0

	for ( i = 0; i < 8; i ++ ) {
	(( ucCmd >>( 7 - i )) & 0x01) ? GPIO_SetBits(GPIOC,GPIO_Pin_3) : GPIO_ResetBits(GPIOC,GPIO_Pin_3);
	delay_us(5);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);//SCLK=1
	delay_us(5);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);//SCLK=0
 
 
	}
}

/*SPI读数据，读取的是12个位，所以循环12次*/
static uint16_t XPT2046_ReadCMD(void)
{
	uint8_t i;
	uint16_t usBuf=0, usTemp;
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_3); //MOSI=0
	GPIO_SetBits(GPIOC,GPIO_Pin_0); //SCLK=1
	
	for ( i=0; i<12; i++ ) {
		GPIO_ResetBits(GPIOC,GPIO_Pin_0); //SCLK=0
		usTemp = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2);//读MISO电平
		usBuf |= usTemp << ( 11 - i );
		GPIO_SetBits(GPIOC,GPIO_Pin_0); //SCLK=1
	}
	
	return usBuf;
}

/*采集触摸屏原始数据
* ucChannel = 0x90 :通道 Y+的选择控制字
* ucChannel =0xd0 :通道 X+的选择控制字
* return 该通道的 ADC 采样结果
*/
static uint16_t XPT2046_ReadAdc ( uint8_t ucChannel )
{
	XPT2046_WriteCMD ( ucChannel );
	return XPT2046_ReadCMD ();
}

/*
* 读取 XPT2046 的 X 通道和 Y 通道的 AD 值（ 12 bit，最大是 4096）
* X_Ad ：存放 X 通道 AD 值的地址
* Y_Ad ：存放 Y 通道 AD 值的地址
*/
static void XPT2046_ReadAdc_XY ( uint16_t * X_Ad, uint16_t * Y_Ad )
{
	
	uint16_t tempx = 0, tempy = 0;
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_13); //CS=0 拉低片选
	delay_us(1);
	tempx = XPT2046_ReadAdc ( 0xd0 );//ReadAdc函数写了 0xd0 是x通道
	delay_us(1);
	tempy = XPT2046_ReadAdc ( 0x90 );//ReadAdc函数写了 0x90 是y通道
	GPIO_SetBits(GPIOC,GPIO_Pin_13); //CS=1	拉高片选，如果你一直读触摸屏数据，可以不用拉高
	
	*X_Ad = tempx;
	*Y_Ad = tempy;
}

/*对获取的X_Ad和Y_Ad进行算法滤波得到真正的X,Y值
*X:真正滤波后的x值返回
*Y:真正滤波后的y值返回
*/
void XPT2046_ReadAdc_Smooth_XY(uint16_t *X , uint16_t *Y)
{
	uint16_t sAD_X = 0, sAD_Y = 0;
	uint16_t xbuffer[10] = {0};
	uint16_t ybuffer[10] = {0};
	uint8_t count = 0;
	int i = 0 ;	
	uint16_t Xmax = 0,Xmin = 0,Ymax = 0,Ymin = 0;
	
	while((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1) == 0) && count < 10) //如果触摸屏被按着，PEN引脚输出低电平
	{
		XPT2046_ReadAdc_XY(&sAD_X,&sAD_Y); //采集ADC X和Y值
		
		xbuffer[count] = sAD_X;
		ybuffer[count] = sAD_Y;		
		count++;		
	}	
	if(count > 9)
	{
		Xmax = Xmin = xbuffer[0];
		Ymax = Ymin = ybuffer[0];
		
		for(i = 1; i < 10; i++) //去除x的最大值和最小值然后给xbuffer
		{
			if(xbuffer[i] < Xmin)
				Xmin = xbuffer[i];
			else
				Xmax = xbuffer[i];
		}
		
		for(i = 1; i < 10; i++) //去除y的最大值和最小值然后给ybuffer
		{
			if(ybuffer[i] < Ymin)
				Ymin = ybuffer[i];
			else
				Ymax = ybuffer[i];
		}
		
		/*计算平均值滤波*/
		*X = xbuffer[0];
		*Y = ybuffer[0];
		for(i = 0; i < 10; i++)
		{
			
			*X = *X + xbuffer[i];
			*Y = *Y + ybuffer[i];
		}
		
		*X = (int)(*X/10);
		*Y = (int)(*Y/10);		
	}
}





