#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"

#include "stdio.h"


/*IOģ��SPI��ʼ��*/
void AnalogSpi_init(void);

/*SPIд������*/
static void XPT2046_WriteCMD ( uint8_t ucCmd );

/*SPI�����ݣ���ȡ����12��λ������ѭ��12��*/
static uint16_t XPT2046_ReadCMD(void);

/*�ɼ�������ԭʼ����
* ucChannel = 0x90 :ͨ�� Y+��ѡ�������
* ucChannel =0xd0 :ͨ�� X+��ѡ�������
* return ��ͨ���� ADC �������
*/
static uint16_t XPT2046_ReadAdc ( uint8_t ucChannel );

/*
* ��ȡ XPT2046 �� X ͨ���� Y ͨ���� AD ֵ�� 12 bit������� 4096��
* X_Ad ����� X ͨ�� AD ֵ�ĵ�ַ
* Y_Ad ����� Y ͨ�� AD ֵ�ĵ�ַ
*/
static void XPT2046_ReadAdc_XY ( uint16_t * X_Ad, uint16_t * Y_Ad );

int main(void)
{
	uint16_t Xvalue = 0, Yvalue = 0;
	   	  	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	AnalogSpi_init();//ģ��SPI IO��ʼ��
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








/*IOģ��SPI��ʼ��*/
void AnalogSpi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_0|GPIO_Pin_13;//��ʼPC0(SCLK),PC3(MOSI),PC13(CS)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;//��ʼPC1(�ж�PEN),PC2(MISO)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}



/*SPIд������*/
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

/*SPI�����ݣ���ȡ����12��λ������ѭ��12��*/
static uint16_t XPT2046_ReadCMD(void)
{
	uint8_t i;
	uint16_t usBuf=0, usTemp;
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_3); //MOSI=0
	GPIO_SetBits(GPIOC,GPIO_Pin_0); //SCLK=1
	
	for ( i=0; i<12; i++ ) {
		GPIO_ResetBits(GPIOC,GPIO_Pin_0); //SCLK=0
		usTemp = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2);//��MISO��ƽ
		usBuf |= usTemp << ( 11 - i );
		GPIO_SetBits(GPIOC,GPIO_Pin_0); //SCLK=1
	}
	
	return usBuf;
}

/*�ɼ�������ԭʼ����
* ucChannel = 0x90 :ͨ�� Y+��ѡ�������
* ucChannel =0xd0 :ͨ�� X+��ѡ�������
* return ��ͨ���� ADC �������
*/
static uint16_t XPT2046_ReadAdc ( uint8_t ucChannel )
{
	XPT2046_WriteCMD ( ucChannel );
	return XPT2046_ReadCMD ();
}

/*
* ��ȡ XPT2046 �� X ͨ���� Y ͨ���� AD ֵ�� 12 bit������� 4096��
* X_Ad ����� X ͨ�� AD ֵ�ĵ�ַ
* Y_Ad ����� Y ͨ�� AD ֵ�ĵ�ַ
*/
static void XPT2046_ReadAdc_XY ( uint16_t * X_Ad, uint16_t * Y_Ad )
{
	
	uint16_t tempx = 0, tempy = 0;
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_13); //CS=0 ����Ƭѡ
	delay_us(1);
	tempx = XPT2046_ReadAdc ( 0xd0 );//ReadAdc����д�� 0xd0 ��xͨ��
	delay_us(1);
	tempy = XPT2046_ReadAdc ( 0x90 );//ReadAdc����д�� 0x90 ��yͨ��
	GPIO_SetBits(GPIOC,GPIO_Pin_13); //CS=1	����Ƭѡ�������һֱ�����������ݣ����Բ�������
	
	*X_Ad = tempx;
	*Y_Ad = tempy;
}





