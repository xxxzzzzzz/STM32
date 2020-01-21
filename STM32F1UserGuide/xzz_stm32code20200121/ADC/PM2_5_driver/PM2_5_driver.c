#include "stm32f10x.h"
#include "pm2_5_driver.h"



/*初始化ADC管脚PA2*/
static void ADC_GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure; //设置GPIO复用ADC功能
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//我们ADC引脚是PA2,所以打开APB2时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//选择PA2  ADCIN2引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//选择PA2引脚为模拟输入,因为使用PA2为ADC采集
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*ADC功能初始化*/
static void ADC_Mode_Config()
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//我们使用ADC1,打开ADC1时钟
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//设置ADC为独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ;//关闭扫描模式，因为我们是单通道ADC采集,不是多通道ADC
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续不停的转换
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//取消外部触发,(外部触发是定时器或者GPIO),我们用代码去触发ADC转换
	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	//我们只用了PA2通道ADC,所以选择1,这个ADC_NbrOfChannel指的是转换通道数量,而不是1个引脚1个通道
	
	ADC_Init(ADC1, &ADC_InitStructure);//使用ADC1外设
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//配置ADC时钟72M/8=9M
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_55Cycles5);//指定规则通道
	
//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//ADC每次转换结束产生中断,不使用，因为我们用的DMA中断
	
	ADC_DMACmd(ADC1,ENABLE);//开启ADC DMA请求
	ADC_Cmd(ADC1, ENABLE);//开启ADC
	
	ADC_ResetCalibration(ADC1);//初始化校准ADC
	while (ADC_GetResetCalibrationStatus(ADC1));//等待初始化ADC1校准完成
	
	ADC_StartCalibration(ADC1);//开始正式校准ADC
	while (ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//打开软件触发ADC			
}



extern __IO uint16_t ADC_ConvertedValue;//存储DMA得到ADC DR寄存器的数据

void ADC_DMA_Init(void)//初始化
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//打开DMA时钟
	
	// ADC1 对应 DMA1 通道 1， ADC3 对应 DMA2 通道 5， ADC2 没有 DMA 功能
	DMA_DeInit(DMA1_Channel1);// 复位 DMA1 通道1 控制器
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&( ADC1->DR ));// 外设基址为： ADC1 数据寄存器地址
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;// 存储器地址
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;// 数据源来自外设
	
	DMA_InitStructure.DMA_BufferSize = 5;// 缓冲区大小，应该等于数据目的地的大小
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// 外设寄存器只有一个，地址不用递增
	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;// 存储器地址不用递增，因为设置了ADC_ConvertedValue 1个变量
	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 外设数据大小为半字，即两个字节
	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;// 内存数据大小也为半字，跟外设数据大小相同
	
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
	






