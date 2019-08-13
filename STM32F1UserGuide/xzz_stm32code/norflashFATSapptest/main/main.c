#include "stm32f10x.h"
#include "sysclock.h"
#include "uartprintf.h"
#include "spi_flash.h"
#include "norflash.h"
#include "stdio.h"
#include "ff.h" //ʹ���ļ�ϵͳ����������ͷ�ļ�
#include<string.h>

FATFS fsp;//�ļ�ϵͳʹ��Ҫ�������������������ܴ����Զ�����ȫ�ֱ���



FRESULT scan_files(char* path)        /* Start node to be scanned (***also used as work area***) */
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
	
    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }
    return res;
}


int main(void)
{
	FRESULT ret; //mount����ֵ������(0)FR_OK��ʾ���سɹ�
	char readbuff[100];//���flash�кܶ��ļ�����������Ҫ������
	
	
	RCC_configuration();//��ʼ��ʱ��
	USART_config(115200);//��ʼ������
	delay_ms(5000);
	
	ret = f_mount(&fsp,"1:",1);//�����ļ�ϵͳ,��norflash���ؽ��ļ�ϵͳ
	if(ret == FR_OK)
		printf("fatfs mount success \r\n");
	else
		printf("fatfs mount failed \r\n");
		
	if(ret == FR_NO_FILESYSTEM)//���norflashû�и�ʽ�����Ǿ͸�ʽ��
	{
		f_mkfs("1:",0,0);
		ret = f_mount(NULL,"1:",1);//��ʽ����ɺ�Ҫȡ������
		ret = f_mount(&fsp,"1:",1);//Ȼ�������¹��ز���ʹ��
		printf("formatting fatfs\r\n");
	}
	
	scan_files("1:");//������̱�ţ�norflash�������õ���1�������ļ���ӡ���� 
		  	
	while(1);
		

	return 0;
	
}












