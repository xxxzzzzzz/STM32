#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"
#include "ff.h" //ʹ���ļ�ϵͳ����������ͷ�ļ�


FATFS fsp;//�ļ�ϵͳʹ��Ҫ�������������������ܴ����Զ�����ȫ�ֱ���
FIL fp;//��д�ļ�Ҫ�����ļ����

int main(void)
{
	FRESULT ret; //mount����ֵ������(0)FR_OK��ʾ���سɹ�
	uint32_t bytenum = 0;
	char INbuf[]="123456,abcdefg";//д���ļ�������
	char OUTbuf[30];
	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	delay_ms(5000);
	
	ret = f_mount(&fsp,"1:",1);//�����ļ�ϵͳ,��norflash���ؽ��ļ�ϵͳ
	printf("res = %d\r\n",ret);
	
	if(ret == FR_NO_FILESYSTEM)//���norflashû�и�ʽ�����Ǿ͸�ʽ��
	{
		f_mkfs("1:",0,0);
		ret = f_mount(NULL,"1:",1);//��ʽ����ɺ�Ҫȡ������
		ret = f_mount(&fsp,"1:",1);//Ȼ�������¹��ز���ʹ��
		printf("res2 = %d\r\n",ret);
	}
	
	
	ret = f_open(&fp,"1:�ļ�ϵͳ�ļ�����.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);//���ļ�
	
	printf("open ret = %d\r\n",ret);
	if(ret == FR_OK)//�ļ��򿪳ɹ�
	{
		printf("open file success\r\n");//���ļ�д������
		ret = f_write(&fp,INbuf,sizeof(INbuf),&bytenum);
		if(ret == FR_OK)
		{
			printf("write file success bytenum = %d\r\n",bytenum);
		}
		else
		{
			printf("write file failed\r\n");
		}
		f_lseek(&fp,0);//�ļ��������ƶ�����ͷ
		ret = f_read(&fp,OUTbuf,f_size(&fp),&bytenum);//�����ļ�������
		if(ret == FR_OK)
		{
			printf("read file success bytenum = %d\r\n",bytenum);
			printf("-----%s----\r\n",OUTbuf);
		}
		else
		{
			printf("read file failed\r\n");
		}
		
	}
	f_close(&fp);//���һ����Ҫ���ˣ����������close���ܵ����ļ���������
	
	while(1);
		

	return 0;
	
}












