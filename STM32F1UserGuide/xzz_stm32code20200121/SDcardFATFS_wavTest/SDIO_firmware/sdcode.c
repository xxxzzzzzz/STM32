#include "sdcode.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Buffer_Block_Tx[512], Buffer_Block_Rx[512];
uint8_t Buffer_MultiBlock_Tx[512*32], Buffer_MultiBlock_Rx[512*32];
static SD_Error Status = SD_OK;//定义一个变量检查SD卡初始化状态



/**
  * @brief  Configures SDIO IRQ channel.
  * @param  None
  * @retval None
  */
void SDIO_NVIC_Configuration(void)//在firmware的C文件唯独没有初始化SDIO中断优先级，所以这里先初始化
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void SD_init()
{
	SDIO_NVIC_Configuration();
	if((Status = SD_Init()) != SD_OK)
	{
		printf("SD init failed \r\n");
	}
	else
	{
		printf("SD init success \r\n");
	}
}




/**
  * @brief  Tests the SD card erase operation.
  * @param  None
  * @retval None
  */
void SD_EraseTest(void)
{
  /*------------------- Block Erase ------------------------------------------*/
  uint16_t index = (512*32);
  if (Status == SD_OK)
  {
    /* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
    Status = SD_Erase(0x00, (512 * 32));//擦除SD卡32个块空间
  }

  if (Status == SD_OK)
  {
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, 512, 32);

    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();

    /* Wait until end of DMA transfer */
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  /* Check the correctness of erased blocks */
  if (Status == SD_OK)
  {
    while (index--)
	{
    /* In some SD Cards the erased state is 0xFF, in others it's 0x00 */
		if ((Buffer_MultiBlock_Rx[index] != 0xFF) && (Buffer_MultiBlock_Rx[index] != 0x00))
		{
			printf("Erase failed\r\n");
		}
	}
	printf("Erase success\r\n");
  } 
}



/**
  * @brief  Tests the SD card Single Blocks operations.
  * @param  None
  * @retval None
  */
void SD_SingleBlockTest(void)
{
  /*------------------- Block Read/Write --------------------------*/
  /* Fill the buffer to send */
  uint16_t index = 0;
  for (index = 0; index < 512; index++)
  {
    Buffer_Block_Tx[index] = 0x32;
  }

  if (Status == SD_OK)
  {
    /* Write block of 512 bytes on address 0 */
    Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE);
	
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }
	
  if (Status == SD_OK)
  {
    /* Read block of 512 bytes from address 0 */
    Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE);
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }
	
  /* Check the correctness of written data */
  index=512;
  while (index--)
  {
    if (Buffer_Block_Tx[index] != Buffer_Block_Rx[index])
    {
      printf("SD single block write failed \r\n");
    }   
  }
  printf("SD single block write success\r\n");
}


/**
  * @brief  Tests the SD card Multiple Blocks operations.
  * @param  None
  * @retval None
  */
void SD_MultiBlockTest(void)
{
  /*--------------- Multiple Block Read/Write ---------------------*/
  /* Fill the buffer to send */

  uint16_t index = 512*32;//512是SD卡一个块(扇区)多少字节，32是要写多少个块
	
  for (index = 0; index < (512*32); index++)
  {
    Buffer_MultiBlock_Tx[index] = 0x32;//Buffer_MultiBlock_Tx是上面定义的512*32大小的数组
  }
  if (Status == SD_OK)
  {
    /* Write multiple block of many bytes on address 0 */
    Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, 512, 32);
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
    /* Read block of many bytes from address 0 */
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, 512, 32);
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  /* Check the correctness of written data */
  if (Status == SD_OK)
  {
    index=512*32;
	while (index--)
	{
		if (Buffer_MultiBlock_Tx[index] != Buffer_MultiBlock_Rx[index])
		{
		printf("SD MultiBlock write failed \r\n");
		}   
	}
	printf("SD MultiBlock write success\r\n");
  }
}


void SDIO_IRQHandler(void)
{
	SD_ProcessIRQSrc();
}
