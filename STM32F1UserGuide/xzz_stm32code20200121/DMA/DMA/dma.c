#include "stm32f10x_dma.h"
#include "dma.h"
#include "uartprintf.h"
#include "sysclock.h"

const uint8_t usart_buffer[32] = "abcdefg123456"; //¼ÓÈëconst Õâ¶ÎÊı×é´æ·ÅÔÚflash

uint32_t readbuff[32];//Ã»ÓĞconstµÄÈ«¾ÖÊı×éÊÇ·ÅÔÚRAMÀïÃæ

//void DMA1_init(void)
//{
//	DMA_InitTypeDef DMA_InitStruct;
//	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//¿ªÆôAHB×ÜÏßDMA1Ê±ÖÓ
//	
//	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)flash_buffer;//´«ÈëflashÊı×éµÄµØÖ·
//	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)readbuff;//ÕâÊÇÒª½ÓÊÜÊı¾İµÄµØÖ·
//	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStruct.DMA_BufferSize = 32;//ÎÒÃÇÒ»´Î´«Êä32¸öÊı¾İ£¬ÒòÎªÊı×éÊÇ32¸
//	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
//	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//Word±íÊ¾1¸öÊı¾İ32Î»
//	
//	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
//	
//	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//flashÊı×éÊı¾İÖ»ÏòRAMÊı×é·¢ËÍÒ»´Î32¸ö
//	DMA_InitStruct.DMA_Priority = DMA_Priority_High; //ÎÒÃÇÖ»Ê¹ÓÃÁËÒ»¸öDMAÍ¨µÀ£¬ÎÒ¾ÍÑ¡×î¸ßÓÅÏÈ¼¶
//	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;//ÒòÎªÊÇflashÊı×é·¢µ½RAMÊı×é£¬ËùÒÔÕâÀïÒª´ò¿ªÄÚ´æµ½ÄÚ´æÄ£Ê½
//	DMA_Init(DMA1_Channel6, &DMA_InitStruct);//Ñ¡ÔñDMA1 Í¨µÀ 6
//	
//	DMA_Cmd(DMA1_Channel6, ENABLE);//´ò¿ªDMA1 Í¨µÀ16
//}

void dma_print(void)
{
	int i = 0;
	for(i=0;i<32;i++)
	{
		printf(" %d ",readbuff[i]);
		delay_ms(100);
	}
}



/*DMA´®¿ÚÊµÑé*/
void DMA1_UART_init(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//¿ªÆôAHB×ÜÏßDMA1Ê±ÖÓ
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(USART1_BASE+0x04);//ÕâÊÇ´®¿ÚÍâÉè»ùµØÖ·+Æ«ÒÆµØÖ·(´®¿ÚÊı¾İ¼Ä´æÆ÷µØÖ·)
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)usart_buffer;//ÕâÊÇ·¢ËÍ¸ø´®¿ÚÊı¾İµÄÊı×é
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//ÄÚ´æ·¢ËÍÊı¾İµ½´®¿ÚÍâÉè
	DMA_InitStruct.DMA_BufferSize = 32;//ÎÒÃÇÒ»´Î´«Êä32¸öÊı¾İ£¬ÒòÎªÊı×éÊÇ32¸
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//ÒòÎª´®¿ÚÖ»ÓĞÒ»¸öÊı¾İ¼Ä´æÆ÷£¬ËùÒÔ²»ĞèÒªÄÚ´æµİÔö
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//Byte±íÊ¾1¸öÊı¾İ8Î»
	
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//ÄÚ´æµİÔöÒª´ò¿ª£¬ÒòÎªÊı×éÊÇµİÔöµÄ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//ÕâÀïµÄÄÚ´æÊı¾İ¶¨ÒåµÄÊÇ8Î»
	
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High; //ÎÒÃÇÖ»Ê¹ÓÃÁËÒ»¸öDMAÍ¨µÀ£¬ÎÒ¾ÍÑ¡×î¸ßÓÅÏÈ¼¶
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//ÒòÎªÊÇflashÊı×é·¢µ½´®¿Ú£¬ËùÒÔÕâÀïÒª¹Ø±ÕÄÚ´æµ½ÄÚ´æÄ£Ê½
	DMA_Init(DMA1_Channel4, &DMA_InitStruct);//¸ù¾İÊı¾İÊÖ²á´®¿Ú1 Ñ¡ÔñDMA1 Í¨µÀ 4
	
	DMA_ClearFlag(DMA1_FLAG_TC4);//Çå³ı·¢ËÍÍê³É±êÖ¾Î»
	
	DMA_Cmd(DMA1_Channel4, ENABLE);//´ò¿ªDMA1 Í¨µÀ4
	
}

