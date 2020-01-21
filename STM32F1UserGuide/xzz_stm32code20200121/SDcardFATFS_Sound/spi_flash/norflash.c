#include "stm32f10x.h"
#include "norflash.h"
#include "spi_flash.h"
#include "uartprintf.h"
#include "stdio.h"

//void SPI_Flash_Read_ID(void)
//{
//	uint32_t flash_ID = 0,subID = 0;
//	
//	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
//	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
//	SPI_send_Byte(JEDEC_ID);
//	flash_ID = SPI_read_Byte();
//	subID = SPI_read_Byte();
//	subID = subID<<8;
//	subID |= SPI_read_Byte();
//	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1	
//	
//	printf("flash ID = %x\r\n",flash_ID);
//	printf("subID    = %x\r\n",subID);
//}


uint32_t SPI_Flash_Read_ID(void)
{
	uint32_t flash_ID = 0,subID = 0;
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	SPI_Send_Read_Byte(JEDEC_ID);
	flash_ID = SPI_Send_Read_Byte(0x00);
	
	subID = SPI_Send_Read_Byte(0x00);
	subID = subID<<8;
	subID |= SPI_Send_Read_Byte(0x00);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1	
	
	flash_ID = ((flash_ID <<16)|subID);
	printf("FLASH ID = %x\r\n",flash_ID);
	return flash_ID;
}



/*��Ϊ����flash�ǳ�����ʱ��,����Ҫд�ȴ�flash�ڲ��������*/
void wait_flass_Erase(void)
{
	uint8_t status_reg = 0;
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(READ_STATUS);//���Ͷ�ȡ״̬�Ĵ�������
	do
	{
		status_reg = SPI_Send_Read_Byte(0x00);//�������õ����ݲ���ʱ�ӣ���flash���ص����ݶ�����
	
	}while((status_reg & 0x01) == 1);//���״̬�Ĵ�����1�ͱ�ʾflash��û�в����ã������ȴ�
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1 flash������ɣ�������CS����	
}

/*����flashָ������*/
void spi_Erase_sector(uint32_t addr)
{
	norflash_Write_ENABLE();//дʹ��
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(ERASE_SECTOR);//����0x20����
	SPI_Send_Read_Byte((addr>>16)&0xff);//���͵�ַ�ĸ�8λ
	SPI_Send_Read_Byte((addr>>8)&0xff);//���͵�ַ��8λ
	SPI_Send_Read_Byte(addr&0xff);//���͵�ַ��8λ
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1 flash��ַд�����
}



/*��ȡflash�������������*/
/*addr:Ҫ��ȡflash�������׵�ַ
* readbuff:����ȡ��һ������4096�ֽ�д��buff��
* byte_number:��ȡ���ٸ��ֽ�
*/
void READ_norflash_sector(uint32_t addr,uint8_t *readbuff,uint32_t byte_number)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(READ_DATA);//���Ͷ�ȡ�����������
	SPI_Send_Read_Byte((addr>>16)&0xff);//���͵�ַ�ĸ�8λ
	SPI_Send_Read_Byte((addr>>8)&0xff);//���͵�ַ��8λ
	SPI_Send_Read_Byte(addr&0xff);//���͵�ַ��8λ
	
	while(byte_number--)
	{
		*readbuff = SPI_Send_Read_Byte(0x00);//�����޹����ݲ���ʱ�ӣ����ܶ�ȡ
		readbuff++;//buff�ƶ�����һ����ַ
	}
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
}


/*norflashдʹ��*/
void norflash_Write_ENABLE(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
	SPI_Send_Read_Byte(WRITE_ENABLE);//дʹ������
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
}



/*д���ݽ�flash����*/
/*addr:Ҫд��flash��������ʼ��ַ
* writebuff:��256���ֽ�д��flash�����Ļ���
* byte_number:д���ٸ��ֽ�
*/
void WRITE_norflash_sector(uint32_t addr,uint8_t *writebuff,uint32_t byte_number)
{
	if(byte_number > 256)//�ж����д�����256���ֽھͱ����˳�����
	{
		printf("write flash data error\r\n");
		return;
	}
	else
	{
		spi_Erase_sector(addr);//д����֮ǰҪ��������
		wait_flass_Erase();//�ȴ�flash�ڲ��������
	
		norflash_Write_ENABLE();//������������֮��д����ҲҪʹ��
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);//CS=0
	
		SPI_Send_Read_Byte(WRITE_DATA);//����д��������
		SPI_Send_Read_Byte((addr>>16)&0xff);//���͵�ַ�ĸ�8λ
		SPI_Send_Read_Byte((addr>>8)&0xff);//���͵�ַ��8λ
		SPI_Send_Read_Byte(addr&0xff);//���͵�ַ��8λ
	
		while(byte_number--)
		{
			SPI_Send_Read_Byte(*writebuff);//����ÿһ���ֽ�д��flash����
			writebuff++;//buff�ƶ�����һ����ַ
		}
	
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//CS=1
	}
	
}



/*д���ݽ�flash�������*/
/*WriteAddr:Ҫд��flash��������ʼ��ַ
* writebuff:�����������ֽ�д��flash�����Ļ���
* byte_number:д���ٸ��ֽ�
*/
void WRITE_norflash_mulitsector(uint32_t WriteAddr,uint8_t *writebuff,uint32_t byte_number)
{
	uint8_t page = 0,addr = 0,count = 0,nofullpage = 0;
	
	addr = WriteAddr%256;
	count = 256 - addr;
	page = byte_number / 256;//����д����ҳ
	nofullpage = byte_number % 256;//����ʣ�²���һҳ���ֽ���
	
	if(addr == 0)//��ַ����
	{
		if(page == 0)//���byte_number��ֵ����4096����С��256���Ͱ���256ҳд����
		{
			WRITE_norflash_sector(WriteAddr,writebuff,byte_number);//�������һ��ֻ��256�ֽ�
		}
		else
		{
			while(page--)
			{
				WRITE_norflash_sector(WriteAddr,writebuff,256);
				WriteAddr+=256;
				writebuff+=256;
			}
			WRITE_norflash_sector(WriteAddr,writebuff,nofullpage);//�����ʣ�µĶ��಻��256�ֽڵ�����д��
		}
	}
	else
	{
		byte_number= byte_number - count;
		page = byte_number / 256;//����д����ҳ
		nofullpage = byte_number % 256;//����ʣ�²���һҳ���ֽ���
		
		WRITE_norflash_sector(WriteAddr,writebuff,count);
		WriteAddr = WriteAddr + count;
		writebuff = writebuff + count;
		
		while(page--)
		{
			WRITE_norflash_sector(WriteAddr,writebuff,256);
			WriteAddr = WriteAddr + 256;
			writebuff = writebuff + 256;
		}
		if(nofullpage !=0 )
		{
			WRITE_norflash_sector(WriteAddr,writebuff,nofullpage);//����ж��಻��һҳ�����ݾͰ���д��
		}
	}
}


