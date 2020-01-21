#include "i2s.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//I2S ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/24
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//V1.1 20140606
//����I2S2ext_Init��I2S2ext_RX_DMA_Init��I2S_Rec_Start��I2S_Rec_Stop�Ⱥ���
////////////////////////////////////////////////////////////////////////////////// 	
 
 
uint8_t dmaTxBuf[512]; 
 
void NVIC_I2S(void); 

//I2S2��ʼ��
//����I2S_Standard:  @ref SPI_I2S_Standard  I2S��׼,
//I2S_Standard_Phillips,�����ֱ�׼;
//I2S_Standard_MSB,MSB�����׼(�Ҷ���);
//I2S_Standard_LSB,LSB�����׼(�����);
//I2S_Standard_PCMShort,I2S_Standard_PCMLong:PCM��׼
//����I2S_Mode: @ref SPI_I2S_Mode  I2S_Mode_SlaveTx:�ӻ�����;I2S_Mode_SlaveRx:�ӻ�����;I2S_Mode_MasterTx:��������;I2S_Mode_MasterRx:��������;
//����I2S_Clock_Polarity  @ref SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,ʱ�ӵ͵�ƽ��Ч;I2S_CPOL_High,ʱ�Ӹߵ�ƽ��Ч
//����I2S_DataFormat�� @ref SPI_I2S_Data_Format :���ݳ���,I2S_DataFormat_16b,16λ��׼;I2S_DataFormat_16bextended,16λ��չ(frame=32bit);I2S_DataFormat_24b,24λ;I2S_DataFormat_32b,32λ.
void I2S3_Init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2S_InitTypeDef I2S_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;//����DMA�ж�
	DMA_InitTypeDef DMA_InitStructure;//����DMA
	
	/* Enable GPIOB, GPIOC and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|  RCC_APB2Periph_GPIOC | 
							RCC_APB2Periph_AFIO , ENABLE);
		
	/* I2S3 SD, CK pins configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* I2S3 WS pins configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* I2S3 MCLK pins configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
		
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);    //��λSPI3
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);   //������λ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//ʹ��SPI3ʱ��,I2S3ʱ����SPI3
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);    //�ر�JTAG���Թ���    
	
	SPI_I2S_DeInit(SPI3);//�Ȼָ�SPI3�Ĵ���Ĭ��ֵ
	
	I2S_InitStructure.I2S_Mode=I2S_Mode_MasterTx;//IIS�������ͣ��ӻ�����
	I2S_InitStructure.I2S_Standard=I2S_Standard_Phillips;//IIS��׼
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat_16b;//IIS���ݳ���
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Enable;//��ʱ�������
	//I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_48k;//IISƵ������(������48K)
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_44k;//44.1K
	I2S_InitStructure.I2S_CPOL=I2S_CPOL_Low;//����״̬ʱ�ӵ�ƽ
	I2S_Init(SPI3,&I2S_InitStructure);//��ʼ��IIS

	//NVIC_I2S();
	
		
			
/*******************************I2S���ӵ�DMA��ʼ��************************************/			
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Channel2);
	//  DMA_StructInit(&DMA_InitStructure);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI3->DR;//0x4000380C;//(uint32_t)(SPI2->DR);  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)dmaTxBuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	DMA_InitStructure.DMA_BufferSize = 512;  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA2_Channel2, &DMA_InitStructure);
	DMA_Cmd(DMA2_Channel2, ENABLE);  
  
	/*����DMA2�ж�ͨ��*/
	DMA_ITConfig(DMA2_Channel2, DMA_IT_TC, ENABLE);
	
  
	
	

  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);       
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);                             /* �������õ���ռ���ȼ��ʹ����ȼ� */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);	
	
	/* Enable the I2S DMA request */
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	
	I2S_Cmd(SPI3,ENABLE);//SPI3 I2S3 ENʹ��.	
} 
 

void NVIC_I2S(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
 /* Set the Vector Table base address at 0x08010000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);	
  
        /* SPI2 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}




void DMA2_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC2))
	{
		DMA_ClearITPendingBit(DMA2_IT_GL2); //���ȫ���жϱ�־

	}
}














/*I2S3�ж����ȼ�����*/
void I2S3_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* SPI3 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}












