#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"

//uint8_t readbuff[4096];//buff��ȫ�����Ͳ��������
//uint8_t writebuff[4096];//д���ݻ�����


//int main(void)
//{
//	
//	uint32_t i=0;
//	RCC_configuration();//��ʼ��ʱ��
//	USART_config(115200);//��ʼ������
//	spi_FUN_init();//SPI��ʼ��

//	delay_ms(5000);
//	
//	for(i=0;i<4096;i++)
//	{
//		writebuff[i] = i;
//	}
//	
//	WRITE_norflash_mulitsector(0,writebuff,4096);
//	
//	while(1){
//		
//		
//		READ_norflash_sector(0,readbuff,4096);//��flash��0����4096�ֽڶ�����
//		
//		for(i=0;i<4096;i++)
//		{
//			printf("0x%x  ",readbuff[i]);
//			if(i%10 == 0) //����ֻ�������ݻ��У��ÿ�Щ
//				printf("\r\n");
//		}	
//		delay_ms(5000);
//	}
//	
//	return 0;
//	
//}


uint8_t writestr[4096]="1,2,3,4,5,6,7,8,9,10,\r\n\
11,12,13,14,15,16,17,18,\r\n\
19,20,21,22,23,24,25,26,27,\r\n\
28,29,30,31,32,33,34,35,36,\r\n\
37,38,39,40,41,42,43,44,45,\r\n\
46,47,48,49,50,51,52,53,54,\r\n\
55,56,57,58,59,60,61,62,63,\r\n\
64,65,66,67,68,69,70,71,72,\r\n\
73,74,75,76,77,78,79,80,81,\r\n\
82,83,84,85,86,87,88,89,90,\r\n\
91,92,93,94,95,96,97,98,99,\r\n\
100,101,102,103,104,105,106,\r\n\
107,108,109,110,111,112,113,\r\n\
114,115,116,117,118,119,120,\r\n\
121,122,123,124,125,126,127,\r\n\
128,129,130,131,132,133,134,\r\n\
135,136,137,138,139,140,141,\r\n\
142,143,144,145,146,147,148,\r\n\
149,150,151,152,154,155,156,\r\n\
157,158,159,160,161,162,163,\r\n\
164,165,166,167,168,169,170,\r\n\
171,172,173,174,175,176,177,\r\n\
178,179,180,181,182,183,184,\r\n\
185,186,187,188,189,190,191,\r\n\
192,193,194,195,196,197,198,\r\n\
199,200,201,202,203,204,256,\r\n\
300,301,302,303,304,305,306";

uint8_t readstr[4096];

int main(void)
{
	
	uint32_t i=0;
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	spi_FUN_init();//SPI��ʼ��

	delay_ms(5000);
	
	WRITE_norflash_mulitsector(0,writestr,4096);//���ַ���д��norflash
	
	while(1)
	{
		READ_norflash_sector(0,readstr,4096);//��flash��0����4096�ֽڶ�����
		printf("----------%s--------\r\n",readstr);//�������256�ֽڵ��ַ�������
		delay_ms(5000);
	}
	
	return 0;
	
}








