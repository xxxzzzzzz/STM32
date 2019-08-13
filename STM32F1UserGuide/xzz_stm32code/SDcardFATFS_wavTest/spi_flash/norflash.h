#ifndef __NORFLASH_H
#define __NORFLASH_H
#include "stm32f10x.h"

#define JEDEC_ID 0x9f
#define ERASE_SECTOR 0x20
#define READ_STATUS 0x05
#define READ_DATA 0x03
#define WRITE_ENABLE 0x06
#define WRITE_DATA 0x02

uint32_t SPI_Flash_Read_ID(void);//���norflash ID
void wait_flass_Erase(void);//�ȴ�flash�������
void spi_Erase_sector(uint32_t addr);//д��Ҫ����flash�������׵�ַ

/*��ȡflash�������������*/
/*addr:Ҫ��ȡflash�������׵�ַ
* readbuff:����ȡ��һ������4096�ֽ�д��buff��
* byte_number:��ȡ���ٸ��ֽ�
*/
void READ_norflash_sector(uint32_t addr,uint8_t *readbuff,uint32_t byte_number);

/*norflashдʹ��*/
void norflash_Write_ENABLE(void);

/*д���ݽ�flash����*/
/*addr:Ҫд��flash��������ʼ��ַ
* writebuff:��256���ֽ�д��flash�����Ļ���
* byte_number:д���ٸ��ֽ�
*/
void WRITE_norflash_sector(uint32_t addr,uint8_t *writebuff,uint32_t byte_number);


/*д���ݽ�flash�������*/
/*WriteAddr:Ҫд��flash��������ʼ��ַ
* writebuff:�����������ֽ�д��flash�����Ļ���
* byte_number:д���ٸ��ֽ�
*/
void WRITE_norflash_mulitsector(uint32_t WriteAddr,uint8_t *writebuff,uint32_t byte_number);

#endif

