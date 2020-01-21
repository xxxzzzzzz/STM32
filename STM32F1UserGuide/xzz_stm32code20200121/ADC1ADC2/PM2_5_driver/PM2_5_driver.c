#include "stm32f10x.h"
#include "pm2_5_driver.h"



/*��ʼ��ADC�ܽ�PA2*/
static void ADC_GPIO_init()
{
	GPIO_InitTypeDef GPIO_InitStructure; //����GPIO����ADC����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����ADC������PA2,���Դ�APB2ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//ѡ��PA2  ADCIN2����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ѡ��PA2����Ϊģ������,��Ϊʹ��PA2ΪADC1�ɼ�
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����ADC������PA3,���Դ�APB2ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//ѡ��PA3  ADCIN3����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ѡ��PA3����Ϊģ������,��Ϊʹ��PA3ΪADC2�ɼ�
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*ADC���ܳ�ʼ��*/
static void ADC_Mode_Config()
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitTypeDef ADC_InitStructure2;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//����ʹ��ADC1,��ADC1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);//����ʹ��ADC2,��ADC2ʱ��
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult ;//��Ϊ��˫ADC������ADCΪ����ͬ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ;//��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//������ͣ��ת��
	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//ȡ���ⲿ����,(�ⲿ�����Ƕ�ʱ������GPIO),�����ô���ȥ����ADCת��
	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	//����ֻ����PA2ͨ��ADC,����ѡ��1,���ADC_NbrOfChannelָ����ת��ͨ������,������ADC������ADC1��ADC2���ǲɼ�1��ͨ��
	
	ADC_Init(ADC1, &ADC_InitStructure);//ʹ��ADC1����
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//����ADCʱ��72M/8=9M  ����1�ι�ADC1,ADC2
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,1,ADC_SampleTime_55Cycles5);//ָ������ͨ��ת��˳�� PA2
	
	ADC_DMACmd(ADC1,ENABLE);//����ADC1 DMA����ֻ��ADC1��DMA����ʹ�ܣ�ADC2û��DMA
	
	/****************ADC1�����꣬��ʼ��ADC2**************/
	
	ADC_InitStructure2.ADC_Mode = ADC_Mode_RegSimult ;//��ADC1����һ��
	ADC_InitStructure2.ADC_ScanConvMode = DISABLE ;//��ADC1����һ��
	ADC_InitStructure2.ADC_ContinuousConvMode = ENABLE;//��ADC1����һ��
	
	ADC_InitStructure2.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ADC1����һ��
	
	ADC_InitStructure2.ADC_DataAlign = ADC_DataAlign_Right;//��ADC1����һ��
	ADC_InitStructure2.ADC_NbrOfChannel = 1;//��ADC1����һ��	
	
	ADC_Init(ADC2, &ADC_InitStructure);//��ADC1����һ��	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	ADC_RegularChannelConfig(ADC2,ADC_Channel_3,1,ADC_SampleTime_55Cycles5);//��ADC1����һ��,��Ϊ��PA3�ɼ���ͨ���ĳ�3
	
	ADC_ExternalTrigConvCmd(ADC2,ENABLE);//ADC2������ADC1�����������������ֻ������Ϊ�ⲿ����
	//ADC2����Դ������ADC1�Ĵ������������ⲿ����
	
	/**********************************************************/
	
//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//ADCÿ��ת�����������ж�,��ʹ�ã���Ϊ�����õ�DMA�ж�
		
	/**************ADC1***************/
	ADC_Cmd(ADC1, ENABLE);//����ADC1
	ADC_ResetCalibration(ADC1);//��ʼ��У׼ADC1
	while (ADC_GetResetCalibrationStatus(ADC1));//�ȴ���ʼ��ADC1У׼���
	
	ADC_StartCalibration(ADC1);//��ʼ��ʽУ׼ADC1
	while (ADC_GetCalibrationStatus(ADC1));
	
	
	/****************ADC2******************/
	ADC_Cmd(ADC2, ENABLE);//����ADC2
	ADC_ResetCalibration(ADC2);//��ʼ��У׼ADC2
	while (ADC_GetResetCalibrationStatus(ADC2));//�ȴ���ʼ��ADC2У׼���
	
	ADC_StartCalibration(ADC2);//��ʼ��ʽУ׼ADC2
	while (ADC_GetCalibrationStatus(ADC2));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//���������ADC1
		
}



extern __IO uint32_t ADC_ConvertedValue;//�洢DMA��ADC DR�Ĵ�����������Ϊ��˫ADC,����Ҫ32λ

void ADC_DMA_Init(void)//��ʼ��
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//��DMAʱ��
	
	// ADC1 ��Ӧ DMA1 ͨ�� 1�� ADC3 ��Ӧ DMA2 ͨ�� 5�� ADC2 û�� DMA ����
	DMA_DeInit(DMA1_Channel1);// ��λ DMA1 ͨ��1 ������
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&( ADC1->DR ));// �����ַΪ�� ADC1 ���ݼĴ�����ַ
	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;// �洢����ַ
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;// ����Դ��������
	
	DMA_InitStructure.DMA_BufferSize = 1;// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С,���ADC�ɼ�2��ͨ�������Ҫ�ĳ�2
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// ����Ĵ���ֻ��һ������ַ���õ���
	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;// �洢����ַ���õ�������Ϊ������ADC_ConvertedValue 1������
	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;// ��������Ϊ32λ����4���ֽ�
	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;// �ڴ����ݴ�СҲΪ32λ�����������ݴ�С��ͬ
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ѭ������ģʽ DMA���ϵ��ظ���������
	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ�� DMA ͨ��ʱ�����ȼ����ò�Ӱ��
	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);// ��ʼ�� DMA1 ͨ��1	
	DMA_Cmd(DMA1_Channel1 , ENABLE);// ʹ�� DMA1 ͨ��
		
}


void ADC_init_Cfg(void)
{
	ADC_GPIO_init();
	ADC_Mode_Config();
	ADC_DMA_Init();
}
	






