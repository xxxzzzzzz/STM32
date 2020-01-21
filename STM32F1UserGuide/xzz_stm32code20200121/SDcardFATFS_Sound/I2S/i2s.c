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
	
	/* Enable GPIOB, GPIOC and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|  RCC_APB2Periph_GPIOC | 
							RCC_APB2Periph_AFIO , ENABLE);
	
	/* I2S2 SD, CK pins configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* I2S2 WS pins configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ��SPI2ʱ��,I2S2ʱ����SPI2
	
	SPI_I2S_DeInit(SPI2);
	
	I2S_InitStructure.I2S_Mode=I2S_Mode_MasterRx;//IIS�ӻ�������������
	I2S_InitStructure.I2S_Standard=I2S_Standard_Phillips;//IIS��׼
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat_16b;//IIS���ݳ���
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//��ʱ������ر�
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_48k;//IISƵ������(������48K)
	I2S_InitStructure.I2S_CPOL=I2S_CPOL_Low;//����״̬ʱ�ӵ�ƽ
	I2S_Init(SPI2,&I2S_InitStructure);//��ʼ��IIS

	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);//���ȹر�I2S���������ж�
	//SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA����ʹ��.
	I2S_Cmd(SPI2,ENABLE);//SPI2 I2S2 ENʹ��.	
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);//I2S2�����ж�ʹ��
	
	
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












