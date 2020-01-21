#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "stdio.h"

#include "ff.h"
#include "sdcode.h"

FATFS fsp;//�ļ�ϵͳʹ��Ҫ�������������������ܴ����Զ�����ȫ�ֱ���
FIL fp;//��д�ļ�Ҫ�����ļ����

extern SD_CardInfo SDCardInfo;

int main(void)
{
	char readbuf[30];//��ȡ�ļ����ݵĻ�����
	FRESULT ret; //mount����ֵ������(0)FR_OK��ʾ���سɹ�
	uint32_t bytenum = 0;
	
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
	printf("SD card sector size = %d\r\n",SDCardInfo.CardBlockSize);
	
	ret = f_open(&fp,"0:SD�������ļ�.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//���ļ�
	
	printf("open ret = %d\r\n",ret);
//	if(ret == FR_OK)//�ļ��򿪳ɹ�
//	{
//		printf("open file success\r\n");//���ļ�д������
//		ret = f_write(&fp,INbuf,sizeof(INbuf),&bytenum);
//		if(ret == FR_OK)
//		{
//			printf("write file success bytenum = %d\r\n",bytenum);
//		}
//		else
//		{
//			printf("write file failed\r\n");
//		}
//	}
//	f_close(&fp);//���һ����Ҫ���ˣ����������close���ܵ����ļ���������
	
	if(ret == FR_OK)//�ļ��򿪳ɹ�
	{
		printf("open file success\r\n");
		f_lseek(&fp,0);
		ret = f_read(&fp,readbuf,f_size(&fp),&bytenum);//��ȡ�ļ�����
		if(ret == FR_OK)
		{
			printf("read file success bytenum = %d\r\n",bytenum);
		}
		else
		{
			printf("read file failed\r\n");
		}
		printf("------------%s----------\r\n",readbuf);
	}
	f_close(&fp);//���һ����Ҫ���ˣ����������close���ܵ����ļ���������
	printf("file system test end\r\n");
	//while(1);	
	return 0;	
}







