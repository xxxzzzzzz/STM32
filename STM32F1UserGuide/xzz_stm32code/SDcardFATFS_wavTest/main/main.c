#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

#include "ff.h"
#include "sdcode.h"
#include "wavfmt.h"

#include <malloc.h>

FATFS fsp;//�ļ�ϵͳʹ��Ҫ�������������������ܴ����Զ�����ȫ�ֱ���
extern SD_CardInfo SDCardInfo;

int main(void)
{
	FIL fp;//��д�ļ�Ҫ�����ļ����
	
	FRESULT ret; //mount����ֵ������(0)FR_OK��ʾ���سɹ�
	uint32_t bytenum = 0,i = 0;
	
	__WaveHeader *wavhead=0;
	u32 wavsize = 0;//WAV�ļ����ݴ�С�����ǲ�����wavͷ���ݴ�С
	
	uint8_t buff[12] = {0xff,1,2,3,4,5,6,7,8,9,10,11};//��������Ƶ����
	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	
	delay_ms(5000);
	
	ret = f_mount(&fsp,"0:",1);//�����ļ�ϵͳ,���ýڵ�2��SD�����ؽ��ļ�ϵͳ
	printf("res = %d\r\n",ret);
	if(ret == FR_NO_FILESYSTEM)//���SD��û�и�ʽ�����Ǿ͸�ʽ��
	{
		f_mkfs("0:",0,0);
		ret = f_mount(NULL,"0:",1);//��ʽ����ɺ�Ҫȡ������
		ret = f_mount(&fsp,"0:",1);//Ȼ�������¹��ز���ʹ��
		printf("res2 = %d\r\n",ret);
	}
	
		
	wavhead = (__WaveHeader *)malloc(sizeof(__WaveHeader));//����wavͷ����
	recoder_wav_init(wavhead);//��wavͷдͷ����
	
	ret = f_open(&fp,"0:recorder.wav",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//�򿪴���wav�ļ�
	printf("open ret = %d\r\n",ret);
	if(ret == FR_OK)//�ļ��򿪳ɹ�
		printf("wav open file success\r\n");//wav�ļ������ɹ�
	else
		printf("wav open file failed\r\n");
		
	ret = f_write(&fp,(const void*)wavhead,sizeof(__WaveHeader),&bytenum);//��wavͷд��wav�ļ�
	if(ret == FR_OK)
		printf("wav write RIFF success\r\n");
	else
		printf("wav write RIFF failed\r\n");
	
	
	
	ret = f_write(&fp,(const void*)buff,sizeof(buff),&bytenum);//������ͷд��wav�ļ�
	if(ret == FR_OK)
		printf("wav write data success\r\n");
	else
		printf("wav write data failed\r\n");
		
	wavsize=sizeof(buff);//������Ƶ���ݳ���
	
	wavhead->riff.ChunkSize=wavsize+36;	//��WAVͷ�ͺ������Ƶ���ݼ������ĳ���
	wavhead->data.ChunkSize=wavsize;//���������Ƶ���ݳ���д��wavͷ�����ݳ��ȱ�����
	
	f_lseek(&fp,0);//ƫ�Ƶ�wav�ļ�ͷ,�Ѽ����wav�ļ����ݳ��ȸ��ǽ�ǰ��д���ļ���wavͷ����
	ret = f_write(&fp,(const void*)wavhead,sizeof(__WaveHeader),&bytenum);//�����ݼ�����д��wav�ļ�
	if(ret == FR_OK)
		printf("wav write data size success\r\n");
	else
		printf("wav write data size failed\r\n");
	f_close(&fp);
	
	//while(1);	
	
	return 0;	
}







