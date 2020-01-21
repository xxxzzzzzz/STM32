#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"

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

int main(void)
{
	uint16_t Xvalue = 0, Yvalue = 0;
	   	  	
	RCC_configuration();//初始化时钟
	USART_config(115200);//初始化串口
	AnalogSpi_init();//模拟SPI IO初始化
	delay_ms(3000);
	printf("xxxzzzzzz\r\n");
	while(1)
	{
		XPT2046_ReadAdc_XY ( &Xvalue, &Yvalue );	
		printf("X value = %d Y value = %d\r\n",Xvalue,Yvalue);
		delay_ms(1000);
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





