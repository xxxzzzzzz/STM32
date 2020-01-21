#include "stm32f10x.h"
#include "pm2_5_driver.h"



/*初始化ADC管脚PA2*/
static void ADC_GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure; //设置GPIO复用ADC功能
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//我们ADC引脚是PA2,所以打开APB2时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//选择PA2  ADCIN2引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//选择PA2引脚为模拟输入,因为使用PA2为ADC1采集
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//我们ADC引脚是PA3,所以打开APB2时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//选择PA3  ADCIN3引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//选择PA3引脚为模拟输入,因为使用PA3为ADC2采集
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*ADC功能初始化*/
static void ADC_Mode_Config()
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitTypeDef ADC_InitStructure2;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//我们使用ADC1,打开ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);//我们使用ADC2,打开ADC2时钟
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult ;//因为是双ADC，设置ADC为规则同步模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ;//打开扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续不停的转换
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//取消外部触发,(外部触发是定时器或者GPIO),我们用代码去触发ADC转换
	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	//我们只用了PA2通道ADC,所以选择1,这个ADC_NbrOfChannel指的是转换通道数量,而不是ADC数量，ADC1和ADC2都是采集1个通道
	
	ADC_Init(ADC1, &ADC_InitStructure);//使用ADC1外设
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//配置ADC时钟72M/8=9M  配置1次管ADC1,ADC2
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_55Cycles5);//指定规则通道转换顺序 PA2
	
	ADC_DMACmd(ADC1,ENABLE);//开启ADC1 DMA请求，只有ADC1有DMA所以使能，ADC2没有DMA
	
	/****************ADC1配置完，初始化ADC2**************/
	
	ADC_InitStructure2.ADC_Mode = ADC_Mode_RegSimult ;//和ADC1配置一样
	ADC_InitStructure2.ADC_ScanConvMode = DISABLE ;//和ADC1配置一样
	ADC_InitStructure2.ADC_ContinuousConvMode = ENABLE;//和ADC1配置一样
	
	ADC_InitStructure2.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//和ADC1配置一样
	
	ADC_InitStructure2.ADC_DataAlign = ADC_DataAlign_Right;//和ADC1配置一样
	ADC_InitStructure2.ADC_NbrOfChannel = 1;//和ADC1配置一样	
	
	ADC_Init(ADC2, &ADC_InitStructure);//和ADC1配置一样	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	ADC_RegularChannelConfig(ADC2,ADC_Channel_3,1,ADC_SampleTime_55Cycles5);//和ADC1配置一样,因为是PA3采集，通道改成3
	
	ADC_ExternalTrigConvCmd(ADC2,ENABLE);//ADC2不能像ADC1那样软件触发，所以只有设置为外部触发
	//ADC2触发源来自于ADC1的触发，所以是外部触发
	
	/**********************************************************/
	
//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//ADC每次转换结束产生中断,不使用，因为我们用的DMA中断
		
	/**************ADC1***************/
	ADC_Cmd(ADC1, ENABLE);//开启ADC1
	ADC_ResetCalibration(ADC1);//初始化校准ADC1
	while (ADC_GetResetCalibrationStatus(ADC1));//等待初始化ADC1校准完成
	
	ADC_StartCalibration(ADC1);//开始正式校准ADC1
	while (ADC_GetCalibrationStatus(ADC1));
	
	
	/****************ADC2******************/
	ADC_Cmd(ADC2, ENABLE);//开启ADC2
	ADC_ResetCalibration(ADC2);//初始化校准ADC2
	while (ADC_GetResetCalibrationStatus(ADC2));//等待初始化ADC2校准完成
	
	ADC_StartCalibration(ADC2);//开始正式校准ADC2
	while (ADC_GetCalibrationStatus(ADC2));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//打开软件触发ADC1
		
}



extern __IO uint32_t ADC_ConvertedValue;//存储DMA到ADC DR寄存器的数据因为是双ADC,所以要32位

void ADC_DMA_Init(void)//初始化
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//打开DMA时钟
	
	// ADC1 对应 DMA1 通道 1， ADC3 对应 DMA2 通道 5， ADC2 没有 DMA 功能
	DMA_DeInit(DMA1_Channel1);// 复位 DMA1 通道1 控制器
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&( ADC1->DR ));// 外设基址为： ADC1 数据寄存器地址
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;// 存储器地址
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;// 数据源来自外设
	
	DMA_InitStructure.DMA_BufferSize = 1;// 缓冲区大小，应该等于数据目的地的大小,如果ADC采集2个通道这里就要改成2
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// 外设寄存器只有一个，地址不用递增
	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;// 存储器地址不用递增，因为设置了ADC_ConvertedValue 1个变量
	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;// 外设数据为32位，即4个字节
	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;// 内存数据大小也为32位，跟外设数据大小相同
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 循环传输模式 DMA不断的重复传输数据
	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;// DMA 传输通道优先级为高，当使用一个 DMA 通道时，优先级设置不影响
	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;// 禁止存储器到存储器模式，因为是从外设到存储器
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);// 初始化 DMA1 通道1	
	DMA_Cmd(DMA1_Channel1 , ENABLE);// 使能 DMA1 通道
		
}


void ADC_init_Cfg(void)
{
	ADC_GPIO_init();
	ADC_Mode_Config();
	ADC_DMA_Init();
}
	






