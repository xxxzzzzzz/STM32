#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"
#include "ff.h"
#include "sdcode.h"
#include "wavfmt.h"
#include "wm8978.h"
#include "i2s.h"
#include "wmxtest.h"


extern SD_CardInfo SDCardInfo;


uint16_t I2S3_Buffer_Rx[32];
FRESULT ret; //mount����ֵ������(0)FR_OK��ʾ���سɹ�

uint32_t bytenum = 0;

__WaveHeader wavhead;
u32 wavsize = 0;//WAV�ļ����ݴ�С�����ǲ�����wavͷ���ݴ�С

int main(void)
{
	FATFS fsp;//�ļ�ϵͳʹ��Ҫ�������������������ܴ����Զ�����ȫ�ֱ���
	FIL fp;//��д�ļ�Ҫ�����ļ����
		
	
	//RCC_configuration();//��ʼ��ʱ��
	RCC_Configuration_I2S();//I2S��ʱ�����ò���
	
	USART_config(115200);//��ʼ������
	//recoder_mode();//��¼��
	I2S3_NVIC_Configuration();//I2S2�ж����ȼ�����
	delay_ms(5000);
	
	I2S3_Init();//�����ֻ�Ͳ¼��
	printf("xxxzzzzzz\r\n");
	
	
	
	ret = f_mount(&fsp,"0:",1);//�����ļ�ϵͳ,���ýڵ�2��SD�����ؽ��ļ�ϵͳ
	printf("res = %d\r\n",ret);
	if(ret == FR_NO_FILESYSTEM)//���SD��û�и�ʽ�����Ǿ͸�ʽ��
	{
		f_mkfs("0:",0,0);
		ret = f_mount(NULL,"0:",1);//��ʽ����ɺ�Ҫȡ������
		ret = f_mount(&fsp,"0:",1);//Ȼ�������¹��ز���ʹ��
		printf("res2 = %d\r\n",ret);
	}
	
		
	//wavhead = (__WaveHeader *)malloc(sizeof(__WaveHeader));//����wavͷ����
	recoder_wav_init(&wavhead);//��wavͷдͷ����
	
	
	
	
	ret = f_open(&fp,"0:recorder.wav",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//�򿪴���wav�ļ�
	printf("open ret = %d\r\n",ret);
	if(ret == FR_OK)//�ļ��򿪳ɹ�
		printf("wav open file success\r\n");//wav�ļ������ɹ�
	else
		printf("wav open file failed\r\n");
		
	ret = f_write(&fp,(const void*)&wavhead,sizeof(__WaveHeader),&bytenum);//��wavͷд��wav�ļ�
	if(ret == FR_OK)
		printf("wav write RIFF success\r\n");
	else
		printf("wav write RIFF failed\r\n");
	
	
	
//	while(1)
//	{
		
		delay_ms(5000);
		wavhead.data.ChunkSize=wavsize;//���������Ƶ���ݳ���д��wavͷ�����ݳ��ȱ�����
		wavhead.riff.ChunkSize=wavsize+36;	//��WAVͷ�ͺ������Ƶ���ݼ������ĳ���
		//f_lseek(&fp,0);//ƫ�Ƶ�wav�ļ�ͷ,�Ѽ����wav�ļ����ݳ��ȸ��ǽ�ǰ��д���ļ���wavͷ����
		//ret = f_write(&fp,(const void*)&wavhead,sizeof(__WaveHeader),&bytenum);//�����ݼ�����д��wav�ļ�
		if(ret == FR_OK)
		{
			//printf("wav write data size success\r\n");
		}
		else
			printf("wav write data size failed\r\n");
		
		printf("wave write end\r\n");
		
		f_close(&fp);		
//	}
	
	
	
	return 0;	
}





__IO uint32_t RxIdx = 0;

void SPI2_IRQHandler(void)
{
	
	//printf("enter i2s3 irq\r\n");
  if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET)
  {
	 SPI_I2S_ClearITPendingBit(SPI2,SPI_I2S_IT_RXNE );//����жϱ�־λ
    /* Store the I2S2 received data in the relative data table */
    I2S3_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
	if(RxIdx == 31)
	{
		//ret = f_write(&fp,(const void*)I2S3_Buffer_Rx,sizeof(I2S3_Buffer_Rx),&bytenum);//������ͷд��wav�ļ�
		if(ret == FR_OK)
		{
			//printf("wav write data success\r\n");
		}
		else
			printf("wav write data failed\r\n");
		wavsize+=sizeof(I2S3_Buffer_Rx);//������Ƶ���ݳ���
		RxIdx = 0;
	}		
			
  }
  
}











