#ifndef __NORFLASH_H
#define __NORFLASH_H
#include "stm32f10x.h"

#define JEDEC_ID 0x9f
#define ERASE_SECTOR 0x20
#define READ_STATUS 0x05
#define READ_DATA 0x03
#define WRITE_ENABLE 0x06
#define WRITE_DATA 0x02

uint32_t SPI_Flash_Read_ID(void);//获得norflash ID
void wait_flass_Erase(void);//等待flash擦除完毕
void spi_Erase_sector(uint32_t addr);//写入要擦除flash扇区的首地址

/*读取flash扇区里面的内容*/
/*addr:要读取flash扇区的首地址
* readbuff:将读取的一个扇区4096字节写入buff，
* byte_number:读取多少个字节
*/
void READ_norflash_sector(uint32_t addr,uint8_t *readbuff,uint32_t byte_number);

/*norflash写使能*/
void norflash_Write_ENABLE(void);

/*写数据进flash扇区*/
/*addr:要写入flash扇区的起始地址
* writebuff:将256个字节写入flash扇区的缓存
* byte_number:写多少个字节
*/
void WRITE_norflash_sector(uint32_t addr,uint8_t *writebuff,uint32_t byte_number);


/*写数据进flash多个扇区*/
/*WriteAddr:要写入flash扇区的起始地址
* writebuff:将任意数量字节写入flash扇区的缓存
* byte_number:写多少个字节
*/
void WRITE_norflash_mulitsector(uint32_t WriteAddr,uint8_t *writebuff,uint32_t byte_number);

#endif

