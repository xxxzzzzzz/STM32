#include "stm32f10x.h"
#include "sram.h"

/*FSMC引脚初始化*/
void init_FSMC_GPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/*初始FSMC管脚时钟
	*GPIOF A0~A9引脚时钟
	*GPIOG A10~A15引脚时钟
	*GPIOD A16~A18引脚时钟 和 D0~D3引脚时钟 D13~D15引脚时钟
	*GPIOE D4~D12引脚时钟
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|
						RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
	/*GPIOG CS片选引脚时钟 我们CS连接的是NE3引脚基地址就是0x68000000
	*GPIOD WE写引脚时钟 ， OE 读引脚时钟
	*GPIOE LD,UD引脚时钟
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOD|
						RCC_APB2Periph_GPIOE,ENABLE);
	
	/*A0~A18地址引脚和D0~D15数据引脚不管读写都配置为复用推挽输出*/
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
	
	/*D0~D15数据线引脚配置*/
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
	
	/*控制信号引脚初始化*/
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


/*FSMC功能配置*/
void FSMC_ModeConfig(void)
{
	FSMC_NORSRAMTimingInitTypeDef readTimInitStruct;
	FSMC_NORSRAMTimingInitTypeDef writeTimInitStruct;
	FSMC_NORSRAMInitTypeDef SRAMInitStruct;
	
	
	RCC_AHBPeriphClockCmd (RCC_AHBPeriph_FSMC ,ENABLE);//启动FSMC时钟
	
	//读时序结构体初始化
	readTimInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;//选择模式A
	readTimInitStruct.FSMC_AddressHoldTime = 0;//SRAM没用到,默认写0
	readTimInitStruct.FSMC_AddressSetupTime = 0; 
	readTimInitStruct.FSMC_BusTurnAroundDuration =0;//SRAM没用到,默认写0
	readTimInitStruct.FSMC_CLKDivision = 0;//SRAM没用到,默认写0
	readTimInitStruct.FSMC_DataLatency = 0;//SRAM没用到,默认写0
	readTimInitStruct.FSMC_DataSetupTime = 2;
		
	//写时序初始化
	writeTimInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;
	writeTimInitStruct.FSMC_AddressHoldTime = 0;//SRAM没用到,默认写0
	writeTimInitStruct.FSMC_AddressSetupTime = 0; 
	writeTimInitStruct.FSMC_BusTurnAroundDuration =0;//SRAM没用到,默认写0
	writeTimInitStruct.FSMC_CLKDivision = 0;//SRAM没用到,默认写0
	writeTimInitStruct.FSMC_DataLatency = 0;//SRAM没用到,默认写0
	writeTimInitStruct.FSMC_DataSetupTime = 2; 
	
	
	SRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	SRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; 
	SRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//SRAM是16位数据口,这里选16b
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
	
	
	SRAMInitStruct.FSMC_ReadWriteTimingStruct = &readTimInitStruct;//把上面设置的读时序放入结构体
	SRAMInitStruct.FSMC_WriteTimingStruct =&writeTimInitStruct; //把上面的写时序放入结构体
	
	
	FSMC_NORSRAMInit(&SRAMInitStruct);//初始化读写时序结构体
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3,ENABLE);//启动Bank1的第3个64M块

}	

