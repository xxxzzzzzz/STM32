#include "stm32f10x.h"
#include "sram.h"

/*FSMC���ų�ʼ��*/
void init_FSMC_GPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/*��ʼFSMC�ܽ�ʱ��
	*GPIOF A0~A9����ʱ��
	*GPIOG A10~A15����ʱ��
	*GPIOD A16~A18����ʱ�� �� D0~D3����ʱ�� D13~D15����ʱ��
	*GPIOE D4~D12����ʱ��
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|
						RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
	/*GPIOG CSƬѡ����ʱ�� ����CS���ӵ���NE3���Ż���ַ����0x68000000
	*GPIOD WEд����ʱ�� �� OE ������ʱ��
	*GPIOE LD,UD����ʱ��
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOD|
						RCC_APB2Periph_GPIOE,ENABLE);
	
	/*A0~A18��ַ���ź�D0~D15�������Ų��ܶ�д������Ϊ�����������*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //A0
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //A1
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //A2
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //A3
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //A4
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //A5
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//A6 
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//A7 
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//A8 
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//A9 
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //A10
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //A11
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //A12
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //A13
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //A14
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //A15
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//A16 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//A17 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//A18 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/*D0~D15��������������*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //D0
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //D1
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //D2
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //D3
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //D4
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //D5
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //D6
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//D7 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//D8 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//D9 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//D10 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//D11 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//D12 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//D13 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//D14 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//D15 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/*�����ź����ų�ʼ��*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //CS
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //WE
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //OE
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //UD
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //LD
	GPIO_Init(GPIOE, &GPIO_InitStructure);
		
}


/*FSMC��������*/
void FSMC_ModeConfig(void)
{
	FSMC_NORSRAMTimingInitTypeDef readTimInitStruct;
	FSMC_NORSRAMTimingInitTypeDef writeTimInitStruct;
	FSMC_NORSRAMInitTypeDef SRAMInitStruct;
	
	
	RCC_AHBPeriphClockCmd (RCC_AHBPeriph_FSMC ,ENABLE);//����FSMCʱ��
	
	//��ʱ��ṹ���ʼ��
	readTimInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;//ѡ��ģʽA
	readTimInitStruct.FSMC_AddressHoldTime = 0;//SRAMû�õ�,Ĭ��д0
	readTimInitStruct.FSMC_AddressSetupTime = 0; 
	readTimInitStruct.FSMC_BusTurnAroundDuration =0;//SRAMû�õ�,Ĭ��д0
	readTimInitStruct.FSMC_CLKDivision = 0;//SRAMû�õ�,Ĭ��д0
	readTimInitStruct.FSMC_DataLatency = 0;//SRAMû�õ�,Ĭ��д0
	readTimInitStruct.FSMC_DataSetupTime = 2;
		
	//дʱ���ʼ��
	writeTimInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;
	writeTimInitStruct.FSMC_AddressHoldTime = 0;//SRAMû�õ�,Ĭ��д0
	writeTimInitStruct.FSMC_AddressSetupTime = 0; 
	writeTimInitStruct.FSMC_BusTurnAroundDuration =0;//SRAMû�õ�,Ĭ��д0
	writeTimInitStruct.FSMC_CLKDivision = 0;//SRAMû�õ�,Ĭ��д0
	writeTimInitStruct.FSMC_DataLatency = 0;//SRAMû�õ�,Ĭ��д0
	writeTimInitStruct.FSMC_DataSetupTime = 2; 
	
	
	SRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	SRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; 
	SRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//SRAM��16λ���ݿ�,����ѡ16b
	SRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	
	
	SRAMInitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable ;
	SRAMInitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable ;
	SRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable ;
	SRAMInitStruct.FSMC_WaitSignal =FSMC_WaitSignal_Disable ;
	SRAMInitStruct.FSMC_WaitSignalActive =FSMC_WaitSignalActive_BeforeWaitState  ;
	SRAMInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low  ;
	SRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
	SRAMInitStruct.FSMC_WriteBurst= FSMC_WriteBurst_Disable;
	SRAMInitStruct.FSMC_WriteOperation= FSMC_WriteOperation_Enable;
	
	
	SRAMInitStruct.FSMC_ReadWriteTimingStruct = &readTimInitStruct;//���������õĶ�ʱ�����ṹ��
	SRAMInitStruct.FSMC_WriteTimingStruct =&writeTimInitStruct; //�������дʱ�����ṹ��
	
	
	FSMC_NORSRAMInit(&SRAMInitStruct);//��ʼ����дʱ��ṹ��
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3,ENABLE);//����Bank1�ĵ�3��64M��

}	

